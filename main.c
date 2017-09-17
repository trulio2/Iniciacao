#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#define Ne 6
#define Ni 0
#define LEN 32
#define Tempo 30000
#define MUT 0.05
#define REC 0.6
#define POP 30
#define pi 3.14159265359

/* Variáveis da Rede */
int times=0,W,L,best=0,i,j,k,t,m,pulos,np,fired[Ne+Ni],firings[Ne+Ni],p,q,trial;
float teta=0,som,pulso,S[Ne+Ni][Ne+Ni],fitness[POP],Fitness,
      gene[POP][LEN],a[Ne+Ni],b[Ne+Ni],c[Ne+Ni],d[Ne+Ni],v[Ne+Ni],u[Ne+Ni],I[Ne+Ni];

/* Variáveis do Jogo */
int teste,obstaculoy,contaux,obstaculos,contobs,obstaculon,pulou,colisoes,cont;
float y,x_obstaculo[10],gravidade,npo,npoo,npooo;

void inicializa()
{
/* Tonic Spiking */
  for (i=2;i<Ne;i++){
    a[i]=.02;
    b[i]=.2;
    c[i]=-65;
    d[i]=6;
  }
/* Class 1 */
  for(i=0;i<2;i++){
    a[i]=.02;
    b[i]=.1;
    c[i]=-55;
    d[i]=6;
  }
/* Cria a Matriz gene com valores aleatórios */
  for (i=0;i<POP;i++)
  {    for (j=0;j<LEN;j++)
         gene[i][j]=drand48();
      fitness[i]=0;
  }

  FILE * popfile = fopen("logPopulation.txt","r");
   for ( i=0;i<POP;i++)
    {    for ( j=0;j<LEN;j++)
           fscanf(popfile,"%f",&(gene[i][j]));
    }
/*
  fclose(popfile);
  FILE * bestGenefile = fopen("logBestGene.txt","r");
   for( n=0;n<LEN;n++)
     fscanf(bestGenefile,"%f",&(gene[0][n]));
  fclose(bestGenefile);
*/

 }

void controle_obstaculos(){
/* Criação de Obstaculos */
  if(k%600==0&&(t-contaux)>900){
      contaux=t;
      x_obstaculo[obstaculoy]=100;
      obstaculoy++;
  }

/* Movimentação dos obstaculos */
  if(obstaculoy>0){
     for(i=0;i<obstaculoy;i++){
        x_obstaculo[i]-=.06;
        //x_obstaculo[i]=  x_obstaculo[i] + (0.001 *-x_obstaculo[i]);
        if(x_obstaculo[i]+5<=0){
            obstaculoy--;
            teste=1;
            contobs=1;
            for(j=i;j<obstaculoy;j++){
               x_obstaculo[j]=x_obstaculo[j+1];
            }
        }
    }
  }

/* Criar a Função do Pulo com relação a frequência do ultimo neurônio aqui */
/*
  if(pulso==1&&cont==0){
    cont=t;
    pulos=1;
  }
  if(pulso==1&&cont!=0){
    pulos++;
  }
  if(t-cont>=50&&pulos>=5){
    pulso=2;
    pulos=0;
    cont=0;
    pulou++;
  }
  if(t-cont>=50&&pulos<5&&pulos>=0){
    cont=0;
    pulso=0;
    pulos=0;
  }
*/
if(pulso==1){
  pulos++;
}
else if(pulso==0  && t%3==0){
  pulos--;
  if(pulos<=0) pulos=0;
}

if(pulos>=10){
  pulso=2;
  pulos=0;
  pulou++;
}

/* Movimentação no eixo Y após o pulo */
  if(pulso>=2||y!=0){
    if(y>=0)
    {
        y=(float)(y+(2-gravidade)/15);
        gravidade=(float)(gravidade+(.2)/15);
        pulso=0;
    }
    if(y<=0)
    {
        gravidade=0;
        y=0;
    }
  }
}

float evaluate(int n){
/* Zera as variáveis usadas em cada teste */
  gravidade = 0;  float Fitness = 0;
  obstaculos = 0; colisoes = 0;
  obstaculoy = 0; m = 0;
  cont = 0;       pulos = 0;
  teste = 1;      np = 0;
  y = 0;          obstaculon = 0;
  pulso = 0;      pulou = 0;
  contobs = 1;    teta = 0;
  contaux=0;      som = 0;

/* Carrega na matriz S os valores do gene na linha n */
  for(i=0;i<Ne;i++){
      for(j=0;j<Ne;j++){
        S[i][j]=0;
        if (i!=j){
            S[i][j]=gene[n][m]*61;
            m++;
        }
      }
  }

/* Tonic Spiking */
  for (i=2;i<Ne+Ni;i++){
    firings[i]=0;
    v[i]=-65;
    u[i]=v[i]*b[i];
  }

/*Class 1 */
  for(i=0;i<2;i++){
    v[i]=-60;
    u[i]=b[i]*v[i];
  }
/* Inicio dos teste com a matriz S atual */
for (t=0;t<Tempo;t++){
  for (j=0;j<Ne+Ni;j++) {
    fired[j]=firings[j];
    firings[j]=0;
  }
  for (j=0;j<Ne;j++) I[j]=0;

/* I na posição 1 recebe o som */
  I[1]=som*(5*gene[n][31]);
  if(I[1]<0) I[1]=0;

/* I na posiao 0 recebe a distancia entre o personagem e o obstaculo, se existir obtaculos na tela */
  if(obstaculoy>0){
    if(x_obstaculo[0]+5>=0&&x_obstaculo[0]<=100){
      I[0]=(1/(6+x_obstaculo[0])) * (50+gene[n][30]*20);
      if(I[0]<0)I[0]=0;
    }
  }
  for(k=0;k<Ne+Ni;k++){
    for(j=0;j<Ne+Ni;j++){
      if(fired[j]==1)I[k]+=S[k][j];
    }
  /* Tonic Spiking */
    if(k>1){
      v[k] += .5*((.04*(pow(v[k],2)))+5*v[k]+140-u[k]+I[k]);
      v[k] += .5*((.04*(pow(v[k],2)))+5*v[k]+140-u[k]+I[k]);
    }
  /* Class 1 */
    if(k<=1){
      v[k]+=.5*((.04*pow(v[k],2))+4.1*v[k]+108-u[k]+I[k]);
      v[k]+=.5*((.04*pow(v[k],2))+4.1*v[k]+108-u[k]+I[k]);
    }
  /* Variável U igual para os dois tipos */
    u[k] += a[k]*(b[k]*v[k]-u[k]);

    if (v[k]>=30){
      firings[k]=1;
      v[k]=c[k];
      u[k]+=d[k];
    }
  }

/*Grava na variavel "pulso" o estado do último neurônio da rede */
  pulso = firings[Ne-1];

/* Cria a variável som com os valores de 0 a 10 conforme uma senóide refletindo a parte negativa */
  som=sin(pi*teta)*10;
  //teta+=.01;
  teta=(float)(teta+(.01)/15);
  if(teta>1) teta=0;

  k=rand();

  controle_obstaculos();

/* Teste de Colisão */
  if((x_obstaculo[0]<=5&&contobs==1)||(x_obstaculo[0]<=5&&teste==1)){
  /* Se colidiu e a variável som é maior ou igual a 5 conta como colisão */
     if(som>=5&&contobs==1){
       obstaculos++;
     }
     if(som>=5&&y<=5&&teste==1){
       colisoes++;
       teste=0;
     }
  /* Se colidiu e a variável som é inferior a 5 não conta como colisão */
     if(som<5&&contobs==1){
       obstaculon++;
       if(y<=5) np++;
     }
     contobs=0;
  }

/* No final do for calcula o Fitness usando as variáveis anteriores */
  if(t==Tempo-1){
    if(obstaculon==0){np=1;obstaculon=1;}
     npo=(float)np/obstaculon;
     npoo=(float)(obstaculos-colisoes)/obstaculos;
     //Fitness=(float)(npoo)*(npo);
     Fitness=(float) ((npoo)*4+(npo))/5.0;
      if(pulou>obstaculos+obstaculon){
         npooo=(float)(pulou-(obstaculos+obstaculon))/(obstaculos+obstaculon);
        Fitness-=(npooo/2);
       }
     if(Fitness>.89)
       printf("%.2f\t%.2f\t%.2f\t%d\t%d\t%d\n",Fitness,npo,npoo,pulou,obstaculos+obstaculon,n);
     if(Fitness<0) Fitness=0;
  }
}
return (Fitness);
}

void trials(){

	time_t t1,t2; (void) time(&t1);
	srand48(time(0)); srand (time(0));
	for ( k=0;k<POP;k++)
		fitness[k]=0;
	FILE * file;
	file = fopen("logGA.txt","w");
	times=0;
	p=POP*drand48();
	q=POP*drand48();
  do{
    fitness[p]=0;
    fitness[q]=0;
    for (trial=0;trial<10;trial++){
  		fitness[p]+=evaluate(p);
  		fitness[q]+=evaluate(q);
    }
    fitness[p]=fitness[p]/10;
    fitness[q]=fitness[q]/10;
		if (fitness[p] > fitness[q])  {W=p; L=q;}
		else                          {W=q; L=p;}
		for ( i=0; i<LEN; i++){
			if (drand48()<REC)
			gene[L][i]=gene[W][i];
			if (drand48()<MUT){
				gene[L][i]+=(0.3*drand48())-0.15;
				if (gene[L][i]>1) gene[L][i]-=1;
				else if (gene[L][i]<0) gene[L][i]+=1;
			}
		}
		p=POP*drand48();
		do{
			q=POP*drand48();
		}while(q==p);

		if (times%100 == 0){
			float fitpopulation;
			fitpopulation=fitness[0];
			best=0;
			for (k=1;k<POP;k++){
				if (fitness[best]<fitness[k])
					best=k;
				fitpopulation+=fitness[k];
			}
			fprintf(file,"%6d %2d %.2f %.2f\n",times,best,fitness[best],fitpopulation/POP);
      printf("\t\t\t%6d %2d %.2f %.2f\n",times,best,fitness[best],fitpopulation/POP);
      FILE * popfile = fopen("logPopulation.txt","w");
        for ( i=0;i<POP;i++){
            for (j=0;j<LEN;j++){
               fprintf(popfile,"%8.6f ",gene[i][j]);
            }
           fprintf(popfile,"\n");
        }
        fclose(popfile);

      FILE * bestGenefile = fopen("logBestGene.txt","w");
    		for (i=0; i<Ne*Ne+1; i++)
    			fprintf(bestGenefile,"%8.6f ",gene[best][i]);
    		fclose(bestGenefile);

		}
    times++;
	}while(fitness[best]<.9);
fclose(file);
}


int main()
{
  inicializa();
  trials();
  return 0;
}
