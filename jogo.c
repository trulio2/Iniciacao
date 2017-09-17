#include <glew.h>
#include <freeglut.h>
#include <SOIL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#define Ne 6
#define Ni 0
#define LEN 32
#define POP 30
#define pi 3.14159265359

/* Variáveis da Rede */
int times=0,W,L,best=0,i,j,k,n,t=0,m,pulos,fired[Ne+Ni],firings[Ne+Ni],p,q,trial;
float teta=0,som,pulso,S[Ne+Ni][Ne+Ni],
      gene[POP][LEN],a[Ne+Ni],b[Ne+Ni],c[Ne+Ni],d[Ne+Ni],v[Ne+Ni],u[Ne+Ni],I[Ne+Ni];

/* Variáveis do Jogo */
int teste,obstaculoy,contaux,contobs,pulou,cont;
float y,x_obstaculo[10],gravidade;

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
/* Cria a Matriz gene com valores aleatórios
  for (i=0;i<POP;i++)
  {    for (j=0;j<LEN;j++)
         gene[i][j]=(rand()%11)/10;
      fitness[i]=0;
  }
  */

  FILE * bestGenefile = fopen("logBestGene.txt","r");
   for( n=0;n<LEN;n++)
     fscanf(bestGenefile,"%f",&(gene[0][n]));
  fclose(bestGenefile);
  glClearColor(0, 0, 0, 0);

    n=0;
/* Zera as variáveis usadas em cada teste */
  gravidade = 0;
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

}
void desenha_tela(){
  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(1, 1, 1);
  glBegin(GL_TRIANGLE_FAN);
    glVertex2d(0,0+y);
    glVertex2d(5,0+y);
    glVertex2d(5,5+y);
    glVertex2d(0,5+y);
  glEnd();

  glColor3f(1, 1, 1);
  glBegin(GL_TRIANGLE_FAN);
    glVertex2d(0,0);
    glVertex2d(100,0);
    glVertex2d(100,1);
    glVertex2d(0,1);
  glEnd();

  if(som>=5){
    glColor3f(1, 1, 1);
    glBegin(GL_TRIANGLE_FAN);
      glVertex2d(0,48);
      glVertex2d(100,48);
      glVertex2d(100,50);
      glVertex2d(0,50);
    glEnd();

  }

  if(obstaculoy>0){
    glColor3f(1, 1, 1);
    glBegin(GL_TRIANGLE_FAN);
      glVertex2d(0+x_obstaculo[0],0);
      glVertex2d(5,0);
      glVertex2d(5,5);
      glVertex2d(0+x_obstaculo[0],5);
    glEnd();
  }

  glutSwapBuffers();
}

void controle_obstaculos(){
/* Criação de Obstaculos */
  if(k%40==0&&(t-contaux)>60){
      contaux=t;
      x_obstaculo[obstaculoy]=100;
      obstaculoy++;
  }

/* Movimentação dos obstaculos */
  if(obstaculoy>0){
     for(i=0;i<obstaculoy;i++){
        x_obstaculo[i]-=.9;
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
  if(pulso==1&&cont==0){
    cont=t;
    pulos=1;
  }
  if(pulso==1&&cont!=0){
    pulos++;
  }
  if(t-cont>=25&&pulos>=3){
    pulso=2;
    pulos=0;
    cont=0;
    pulou++;
  }
  if(t-cont>=50&&pulos<5&&pulos>0){
    cont=0;
    pulso=0;
    pulos=0;
  }

/* Movimentação no eixo Y após o pulo */
  if(pulso>=2||y!=0){
    if(y>=0)
    {
        y=(float)y+(2-gravidade);
        gravidade=(float)gravidade+(.2);
        pulso=0;
    }
    if(y<0)
    {
        gravidade=0;
        y=0;
    }
  }
}

void timer(int idx){

/* Inicio dos teste com a matriz S atual */
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
  teta+=.01;
  if(teta>1) teta=0;

  k=rand();

  controle_obstaculos();

  t++;
  glutPostRedisplay();
  glutTimerFunc(17, timer, 0);
}

void redimensionada(int width, int height)
{   glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(width>height) glViewport((width-height)/2, 0, height, height);
    if(height>width) glViewport(0, (height-width)/2, width, width);
    glMatrixMode(GL_PROJECTION);
    glOrtho(0.0, 100.0, 0.0, 100.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

}

int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Tchaguin exe");
  glutDisplayFunc(desenha_tela);
  glutReshapeFunc(redimensionada);
  inicializa();
  glutTimerFunc(0, timer, 0);
  glutMainLoop();
  return 0;
}
