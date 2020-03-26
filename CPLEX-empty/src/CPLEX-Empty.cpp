#include <ilcplex/ilocplex.h>
#include <iostream>

ILOSTLBEGIN

typedef IloArray<IloIntVarArray> IloIntVarArray2;
typedef IloArray<IloBoolVarArray> IloBoolVarArray2;
typedef IloArray<IloBoolVarArray2> IloBoolVarArray3;

int main(int argv, char *[])
{
	IloEnv env;
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

	//Inicializando Variáveis com as Informações do Arquivo
	char ** NomeDisciplinas = (char**) malloc(nDisciplinas * sizeof(char *));
	for(int d=0; d<nDisciplinas; d++)
		NomeDisciplinas[d] = (char*) malloc(10 * sizeof(char));

	int * DiscTamSlot = (int*) malloc(nDisciplinas * sizeof(int));

	int ** DiscProf = (int**) malloc(nProfessores * sizeof(int *));
	for(int p=0; p<nProfessores; p++)
		DiscProf[p] = (int*) malloc(nDisciplinas * sizeof(int));

	int ** DiscSalas = (int**) malloc(nSalas * sizeof(int *));
	for(int s=0; s<nSalas; s++)
		DiscSalas[s] = (int*) malloc(nDisciplinas * sizeof(int));

	int ** DiscPorTurma = (int**) malloc(nTurmas * sizeof(int *));
	for(int t=0; t<nTurmas; t++)
		DiscPorTurma[t] = (int*) malloc(nDisciplinas * sizeof(int));

	int ** DiscConflitos = (int**) malloc(nDisciplinas * sizeof(int *));
	for(int d=0; d<nDisciplinas; d++)
		DiscConflitos[d] = (int*) malloc(nDisciplinas * sizeof(int));

	int ** DiscIniciaSlot = (int**) malloc(nSlotsDia * sizeof(int *));
	for(int h=0;h<nSlotsDia; h++)
		DiscIniciaSlot[h] = (int*) malloc(nDisciplinas * sizeof(int));


	try
	{
		// Inicializando a Matrizes com 0
		for(int i = 0; i < nDisciplinas; i++)
		{
			for(int j = 0; j < nProfessores; j++)
				DiscProf[j][i] = 0;
			for(int j = 0; j < nSalas; j++)
				DiscSalas[j][i] = 0;
			for(int j = 0; j < nTurmas; j++)
				DiscPorTurma[j][i] = 0;
			for(int j = 0; j < nDisciplinas; j++)
				DiscConflitos[i][j] = 0;
			for(int j = 0; j < nSlotsDia; j++)
				DiscIniciaSlot[j][i] = 0;
		}

		char temp[10];
		for(int i = 0; i < nDisciplinas; i++) {
			arq >> temp; //Sigla da Disciplina
			sprintf(NomeDisciplinas[i],"%s",temp);

			int prof;
			arq >> prof; //Professor vinculado a disciplina
			DiscProf[prof][i] = 1;


			int tamanho;
			arq >> tamanho; //Tamanho dos Slots
			DiscTamSlot[i] = tamanho;

			switch(tamanho)
			{
			case 1:
				DiscIniciaSlot[0][i] = 1;
				DiscIniciaSlot[1][i] = 1;
				DiscIniciaSlot[2][i] = 1;
				DiscIniciaSlot[3][i] = 1;
				DiscIniciaSlot[4][i] = 1;
				break;
			case 2:
				DiscIniciaSlot[0][i] = 1;
				DiscIniciaSlot[2][i] = 1;
				DiscIniciaSlot[3][i] = 1;
				break;
			case 3:
				DiscIniciaSlot[2][i] = 1;
				break;
			}

			cout << "Disciplina " << i << " " << temp << " tamanho " << tamanho << " professor " << prof ;

			int nSalasDisc;
			arq >> nSalasDisc;
			cerr << " Salas " << nSalasDisc << " ->";
			for(int j=0; j < nSalasDisc; j++)
			{
				IloInt idSalasDisc;
				arq >> idSalasDisc;
				DiscSalas[idSalasDisc][i] = 1;
				cout << " " << idSalasDisc;
			}
			cout << endl;
		}

		//Le as informações de turmas
		for(int i = 0; i < nTurmas; i++) {
			for(int j = 0; j < nDisciplinas; j++) {
				cerr << DiscPorTurma[i][j] << " ";
			}
			cerr << endl;
		}

		cerr << " Turmas " << nTurmas << endl;
		for(int i = 0; i < nTurmas; i++) {
			int nDisc;
			arq >> nDisc;
			cerr << "Turma " << i << " Numero Disciplinas " << nDisc << " ->";
			for(int j = 0; j < nDisc; j++) {
				int idDisc;
				arq >> idDisc;
				cerr << "Turma " << i << " j " << j << " idDisc " << idDisc ;
				DiscPorTurma[i][idDisc] = 1;
				//cerr << " conteudo " << DiscPorTurma[i][idDisc] << endl ;
			}
			cerr << endl;
		}

		cerr << " PASSOU DA TURMA" << endl;

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
					DiscConflitos[DiscLista[j]][DiscLista[k]] = 1;
					//DiscConflitos[DiscLista[k]][DiscLista[j]] = 1;
					cout << " " << DiscLista[j] << " " << DiscLista[k] << ",";
				}
			}

			cout << endl;
		}

		arq.close();
		cerr << " PASSOU DO CONFLITO" << endl;

		//Criando Variáveis do Modelo
		//Xhwd se d é dada em horário h no dia w
		IloBoolVarArray3 x(env,nSlotsDia);
		for(int i = 0; i < nSlotsDia; i++) {
			x[i] = IloBoolVarArray2(env, nDiasSemana );
			for(int j = 0; j < nDiasSemana; j++) {
				x[i][j] = IloBoolVarArray(env, nDisciplinas);
			}
		}

		char name[100];
		for(int i = 0; i < nSlotsDia; i++) {
			for(int j = 0; j < nDiasSemana; j++) {
				for(int d = 0; d < nDisciplinas; d++) {
					sprintf(name,"x_%d_%d_%d",i,j,d);
					x[i][j][d].setName(name);
				}
			}
		}


		//Yds se d é dada na sala s
		IloBoolVarArray2 y(env,nDisciplinas);
		for(int i = 0; i < nDisciplinas; i++) {
			y[i] = IloBoolVarArray(env, nSalas );
		}

		for(int i = 0; i < nDisciplinas; i++) {
			for(int j = 0; j < nSalas; j++) {
				sprintf(name,"y_%d_%d",i,j);
				y[i][j].setName(name);
			}
		}

		//Zhwd se d é começa em horário h no dia w
		IloBoolVarArray3 z(env,nSlotsDia);
		for(int i = 0; i < nSlotsDia; i++) {
			z[i] = IloBoolVarArray2(env, nDiasSemana);
			for(int j = 0; j < nDiasSemana; j++) {
				z[i][j] = IloBoolVarArray(env, nDisciplinas);
			}
		}

		for(int i = 0; i < nSlotsDia; i++) {
			for(int j = 0; j < nDiasSemana; j++) {
				for(int d = 0; d < nDisciplinas; d++) {
					sprintf(name,"z_%d_%d_%d",i,j,d);
					z[i][j][d].setName(name);
				}
			}
		}

		cerr << " PASSOU DAS VARIÁVEIS" << endl;

		// Criando o modelo
		IloModel model(env);


		// Entrando com as restrições
		// O mesmo professor não pode dar duas disciplinas em mesmo dia e horário
		for(int p = 0; p < nProfessores; p++) {
			for(int h = 0; h < nSlotsDia; h++) {
				for(int w = 0; w < nDiasSemana; w++) {
					IloExpr numAulaProf(env);
					for(int d = 0; d < nDisciplinas; d++) {
						if(DiscProf[p][d]) {
							numAulaProf += x[h][w][d];
						}
					}
					model.add(numAulaProf <= 1);
				}
			}
		}

		cerr << " PASSOU 1 RESTRICAO" << endl;

		// Duas disciplinas da mesma turma não podem ser dadas no mesmo horário e mesmo dia
		for(int t = 0; t < nTurmas; t++) {
			for(int h = 0; h < nSlotsDia; h++) {
				for(int w = 0; w < nDiasSemana; w++) {
					IloExpr numDiscTurma(env);
					for(int d = 0; d < nDisciplinas; d++) {
						if(DiscPorTurma[t][d]) {
							numDiscTurma += x[h][w][d];
						}
					}
					model.add(numDiscTurma <= 1);
				}
			}
		}

		cout << " PASSOU 2 RESTRICAO" << endl;

		//A disciplina deve começar em horário definido
		for(int d = 0; d < nDisciplinas; d++) {
			IloExpr discInicia(env);
			for(int h = 0; h < nSlotsDia; h++) {
				for(int w = 0; w < nDiasSemana; w++) {
					if(DiscIniciaSlot[h][d]) {
						discInicia += z[h][w][d];
					}
				}
			}
			model.add(discInicia == 1);
		}


		cout << " PASSOU 3 RESTRICAO" << endl;


		//Não pode ter duas disciplinas sendo dadas na mesma sala no mesmo dia e horário
		for(int h = 0; h < nSlotsDia; h++) {
			for(int w = 0; w < nDiasSemana; w++) {
				for(int s = 0; s < nSalas; s++) {
					for(int d = 0; d < nDisciplinas; d++) {
						for(int d2 = 0; d2 < nDisciplinas; d2++) {
							if(d != d2)
								model.add(x[h][w][d] + y[d][s] + x[h][w][d2] + y[d2][s] <= 3);
						}
					}
				}
			}
		}

		cout << " PASSOU 4 RESTRICAO" << endl;

		//A disciplina será dada o tamanho de slots dela
		for(int d = 0; d < nDisciplinas; d++) {
			IloExpr discTamanho(env);
			for(int h = 0; h < nSlotsDia; h++) {
				for(int w = 0; w < nDiasSemana; w++) {
					discTamanho += x[h][w][d];
				}
			}
			model.add(discTamanho == DiscTamSlot[d]);
		}


		cout << " PASSOU 5 RESTRICAO" << endl;


		//Duas disciplinas conflitantes não começam no mesmo dia
		for(int w = 0; w < nDiasSemana; w++) {
			for(int d = 0; d < nDisciplinas; d++) {
				for(int d2 = d + 1; d < nDisciplinas; d++) {
					if(DiscConflitos[d][d2])
					{
						IloExpr discConfInicia(env);
						for(int h = 0; h < nSlotsDia; h++) {
							discConfInicia += z[h][w][d] + z[h][w][d2];
						}
						model.add(discConfInicia <= 1);
					}
				}
			}
		}


		cout << " PASSOU 6 RESTRICAO" << endl;

		//Se uma disc d comeca em um horario h de um dia w entao
		//s(d) slots da mesma disciplina no mesmo dia e horario
		//e ocupado os s(d)-1 horarios seguintes
		for(int d = 0; d < nDisciplinas; d++) {
			for(int w = 0; w < nDiasSemana; w++) {
				for(int h = 0; h < nSlotsDia; h++) {
					if(DiscIniciaSlot[h][d])
					{
						IloExpr discComEhorSeguintes(env);
						for(int h2 = h; h2 < h+DiscTamSlot[d]; h2++)
							discComEhorSeguintes += x[h2][w][d];
						model.add(z[h][w][d]*DiscTamSlot[d] <= discComEhorSeguintes );
					}
				}
			}
		}

		cout << " PASSOU 7 RESTRICAO" << endl;

		//A disciplina deve ser dada em uma sala
		for(int d = 0; d < nDisciplinas; d++) {
			IloExpr discNaSala(env);
			for(int s = 0; s < nSalas; s++) {
				if(DiscSalas[s][d])
				{
					discNaSala += y[d][s];
				}
			}
			model.add(discNaSala == 1);
		}


		cout << " PASSOU 8 RESTRICAO" << endl;

		IloCplex solver(model);
		solver.exportModel("ucp.lp");
		solver.solve();

		cout << "Status solucao: " << solver.getStatus() << endl;

		for(int t=0; t<nTurmas;t++)
		{
			cout << endl << "Turma: " << t << endl;
			cout << "SEGUNDA-FEIRA \t TERÇA-FEIRA \t QUARTA-FEIRA \t QUINTA-FEIRA \t SEXTA-FEIRA" << endl;
			for(int h=0; h< nSlotsDia;h++)
			{
				for(int w=0; w< nDiasSemana;w++)
				{
					bool achou = false;
					for(int d = 0; d < nDisciplinas; d++)
					{
						if(DiscPorTurma[t][d])
						{
							if(solver.getValue(x[h][w][d]))
							{

								for(int p=0;p<nProfessores;p++)
								{
									if(DiscProf[p][d])
									{
										for(int s=0; s < nSalas; s++)
										{
											if(solver.getValue(y[d][s]))
											{
												cout << NomeDisciplinas[d] << " P:" <<  p << " S:" << s << " \t ";
												achou = true;
											}
										}
									}
								}
							}
						}
					}
					if(!achou)
						cout << "Sem Aula \t ";

				}
				cout << endl;
			}
			cout << endl ;
		}

		x.end();
		y.end();
		z.end();


	}catch(IloException& ex){

	}

	free(DiscTamSlot);
	for(int p=0; p<nProfessores; p++)
		free(DiscProf[p]);
	free(DiscProf);

	for(int s=0; s<nSalas; s++)
		free(DiscSalas[s]);
	free(DiscSalas);

	for(int t=0; t<nTurmas; t++)
		free(DiscPorTurma[t]);
	free(DiscPorTurma);

	for(int d=0; d<nDisciplinas; d++)
		free(DiscConflitos[d]);
	free(DiscConflitos);

	for(int h=0;h<nSlotsDia; h++)
		free(DiscIniciaSlot[h]);
	free(DiscIniciaSlot);

	cerr << endl << "Limpou as variáveis" << endl;

	env.end();

}
