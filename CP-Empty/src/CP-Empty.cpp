#include <ilcp/cp.h>
#include <math.h>

#include <iostream>
#include <string.h>

using namespace std;

int array1[25] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};
int array2[15] = {0,2,3,5,7,8,10,12,13,15,17,18,20,22,23};
int array3[5] = {2,7,12,17,22};


int main(int, const char * [])
{
	IloEnv env;
	IloModel model(env);

	IloIntArray SlotsTam1(env, 25);
	for(int i=0; i < 25; i++)
		SlotsTam1.add(array1[i]);
	IloIntArray SlotsTam2(env, 15);
	for(int i=0; i < 15; i++)
		SlotsTam2.add(array2[i]);
	IloIntArray SlotsTam3(env, 5);
	for(int i=0; i < 5; i++)
		SlotsTam3.add(array3[i]);

	try{

		ifstream arq("dados.txt");
		int nSlotsDia, nDiasSemana, nTurmas, nDisciplinas, nProfessores, nSalas, nConflitos;

		// Lê as primeiras informações do arquivo
		arq >> nSlotsDia;
		arq >> nDiasSemana;
		arq >> nProfessores;
		arq >> nSalas;
		arq >> nTurmas;
		arq >> nConflitos;
		arq >> nDisciplinas;

		cout << "Atribuiu primeiras informações " << nSlotsDia << " " << nDiasSemana << " " << nProfessores << " " << nSalas << " " << nDisciplinas << " " << endl;

		IloIntervalVarArray Professores[nProfessores];
		for(int i = 0; i < nProfessores; i++)
			Professores[i] = IloIntervalVarArray(env);

		//Lendo Disciplinas
		IloIntVarArray Inicios(env);
		IloIntervalVar Disciplinas[nDisciplinas];
		IloIntVarArray DiscSalas(env);
		IloIntArray DisciplinaProfessor(env, nDisciplinas);

		for(int i = 0; i < nDisciplinas; i++) {
			string temp = "";
			arq >> temp; //Sigla da Disciplina
			const char * nomeDisc = temp.c_str();

			int prof;
			arq >> prof; //Professor vinculado a disciplina
			DisciplinaProfessor[i] = prof;

			int tamanho;
			arq >> tamanho; //Tamanho dos Slots

			cout << "Disciplina " << i << " " << nomeDisc << " tamanho " << tamanho << " professor " << prof ;

			IloIntArray* ponteiro;
			switch(tamanho)
			{
			case 1:
				ponteiro = &SlotsTam1;
				break;
			case 2:
				ponteiro = &SlotsTam2;
				break;
			case 3:
				ponteiro = &SlotsTam3;
				break;
			}

			string nome = "Inicio " + temp + ":";
			IloIntVar x(env, *ponteiro, nome.c_str());
			Inicios.add(x);
			Disciplinas[i] = IloIntervalVar(env, tamanho, nomeDisc);

			//Adicionando Professor
			model.add(x == IloStartOf(Disciplinas[i])); //Adicionando início dos Slots
			Professores[prof].add(Disciplinas[i]); //Adicionando Professor

			//Adicionando salas
			int nSalasDisc;
			arq >> nSalasDisc;
			cout << " Salas " << nSalasDisc << " ->";
			IloIntArray SalasPossiveis(env);
			for(int j=0; j < nSalasDisc; j++)
			{
				int idSalasDisc;
				arq >> idSalasDisc;
				SalasPossiveis.add(idSalasDisc);
				cout << " " << idSalasDisc;
			}
			string nomesalas = "Salas Disciplina " + temp + ": ";
			IloIntVar s (env, SalasPossiveis, nomesalas.c_str());
			DiscSalas.add(s);

			cout << endl;
		}

		cout << endl << "IMPRIMINDO PROFESSORES/DISCIPLINAS " << endl;
		for(int i = 0; i < nProfessores; i++)
		{
			cout << "Prof: " << i << " ";
			for(int j = 0; j < Professores[i].getSize(); j++ )
			{
				cout << Professores[i][j].getName() << "\t ";
			}
			cout << endl;
		}


		//Le as informações de turmas
		IloIntervalVarArray DiscPorTurma[nTurmas];
		IloIntArray DiscPorTurmaId[nTurmas];
		for(int i = 0; i < nTurmas; i++) {
			DiscPorTurma[i] = IloIntervalVarArray(env);
			DiscPorTurmaId[i] = IloIntArray(env);
			int nDisc;
			arq >> nDisc;
			cerr << "Turma " << i << " Numero Disciplinas " << nDisc << " ->";
			for(int j = 0; j < nDisc; j++) {
				int idDisc;
				arq >> idDisc;
				DiscPorTurma[i].add(Disciplinas[idDisc]);
				DiscPorTurmaId[i].add(idDisc);
				cerr << " " << idDisc;
			}
			cout << endl;
			model.add(IloNoOverlap(env,DiscPorTurma[i]));
		}


		//Le as informações de Conflitos
		for(int i = 0; i < nConflitos; i++) {
			int nDisc;
			arq >> nDisc;
			cout << "Conflitos " << i << " Qtd " << nDisc << " -> ";

			int DiscLista[nDisc];
			for(int j = 0; j < nDisc; j++)
				arq >> DiscLista[j];

			for(int j = 0; j < nDisc -1; j++) {
				for(int k = j+1; k < nDisc; k++) {

					//SEG
					model.add(IloIfThen(env, IloStartOf(Disciplinas[DiscLista[j]]) <= 4, IloStartOf(Disciplinas[DiscLista[k]]) >=5));
					//TER
					model.add(IloIfThen(env, 5 <= IloStartOf(Disciplinas[DiscLista[j]]) <= 9, IloStartOf(Disciplinas[DiscLista[k]]) <=4 || 10 <= IloStartOf(Disciplinas[DiscLista[k]])));
					//QUA
					model.add(IloIfThen(env, 10 <= IloStartOf(Disciplinas[DiscLista[j]]) <= 14, IloStartOf(Disciplinas[DiscLista[k]]) <=9 || 15 <= IloStartOf(Disciplinas[DiscLista[k]])));
					//QUI
					model.add(IloIfThen(env, 15 <= IloStartOf(Disciplinas[DiscLista[j]]) <= 19, IloStartOf(Disciplinas[DiscLista[k]]) <=15 || 20 <= IloStartOf(Disciplinas[DiscLista[k]])));
					//SEX
					model.add(IloIfThen(env, 20 <= IloStartOf(Disciplinas[DiscLista[j]]) <= 24, IloStartOf(Disciplinas[DiscLista[k]]) <=21 ));

					cout << " D[" << DiscLista[j] << "]=" << Disciplinas[DiscLista[j]].getName() << "] D[" << DiscLista[k] << "]=" << Disciplinas[DiscLista[k]].getName() << ",";
				}
			}

			cout << endl;
		}

		//Demais Restrições
		for(int i = 0; i < nProfessores; i++)
			model.add(IloNoOverlap(env,Professores[i]));

		for(int i =0 ; i <nDisciplinas; i++)
		{
			for(int j =0 ; j <nDisciplinas; j++)
			{
				if(i != j)
					model.add(IloIfThen(env, DiscSalas[i] == DiscSalas[j], IloStartOf(Disciplinas[i]) >= IloEndOf(Disciplinas[j]) || IloStartOf(Disciplinas[j]) >= IloEndOf(Disciplinas[i])));
			}
		}
		//Realizando Resolução
		IloCP cp(model);

		cp.exportModel("ucp.cpo");



		if (cp.solve()) {
			cout << endl << "Status solucao: " << cp.getStatus() << endl;

			for(int i = 0; i < nTurmas; i++)
			{
				cout << endl << "Turma " << i << std::endl;
				cout << "SEGUNDA-FEIRA \t TERÇA-FEIRA \t QUARTA-FEIRA \t QUINTA-FEIRA \t SEXTA-FEIRA " << endl;
				for(int j = 0; j < nSlotsDia; j++)
				{
					for(int w = 0; w < nDiasSemana; w++) {
						IloInt c = w * nSlotsDia + j;

						bool achou = false;

						for(int d = 0; d < DiscPorTurma[i].getSize(); d++) {
							int inicio = cp.getValue(Inicios[DiscPorTurmaId[i][d]]);
							int final = (inicio + Disciplinas[DiscPorTurmaId[i][d]].getSizeMax());

							if(inicio <= c && c < final) {
								cout << "D:" << Disciplinas[DiscPorTurmaId[i][d]].getName();
								cout << " S:" << cp.getValue(DiscSalas[DiscPorTurmaId[i][d]]);
								cout << " P:" << DisciplinaProfessor[DiscPorTurmaId[i][d]] << " \t ";
								achou = true;
							}
						}

						if(!achou) {
							cout << "Sem aula" << " \t ";
						}
					}

					cout << endl;

				}
				cout << std::endl;
			}
		} else {
			cp.out() << "No solution found. " << std::endl;
		}

	}catch(IloException& ex)
	{
		env.out() << "Erro: " << ex << std::endl;
	}
}
