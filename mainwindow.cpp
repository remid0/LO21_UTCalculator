#include "mainwindow.h"
#include "ui_mainwindow.h"

//constructeur de la fenetre
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //Creation vuePile
    Pile& P = controleur.getPile();
    ui->vuePile->setRowCount(P.getNbToAffiche());
    ui->vuePile->setColumnCount(1);
    ui->vuePile->setStyleSheet("background: darker; color: white");
    ui->vuePile->verticalHeader()->setStyleSheet("color: black");
    ui->vuePile->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->vuePile->horizontalHeader()->setVisible(false);
    ui->vuePile->horizontalHeader()->setStretchLastSection(true);
    //Creation la vue des variable
        //VueVArID
    ui->vueVarId->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->vueVarId->horizontalHeader()->setVisible(false);
    ui->vueVarId->verticalHeader()->setVisible(false);
    ui->vueVarId->setColumnCount(1);
        //vueVarValue
    ui->vueVarValue->horizontalHeader()->setVisible(false);
    ui->vueVarValue->verticalHeader()->setVisible(false);
    ui->vueVarValue->setColumnCount(1);
        //MessageVar
    ui->messageVar->setStyleSheet("background: cyan; color: black");
    ui->messageVar->setReadOnly(true);
    //Creation du son
    player = new QMediaPlayer();
    player->setMedia(QMediaContent(QUrl::fromLocalFile("beep.mp3")));
    player->setVolume(50);
    //Paramtre de Message
    ui->message->setStyleSheet("background: cyan; color: black");
    ui->message->setReadOnly(true);
    //conection des signaux
    connect(&P,SIGNAL(modificationEtat()),this,SLOT(refreshCalcul()));
    connect(&controleur,SIGNAL(modificationVar()),this,SLOT(refreshVar()));
    connect(ui->commande,SIGNAL(returnPressed()),this,SLOT(getNextCommande()));
    //connect(&controleur,SIGNAL(pressedOperator()),this,SLOT(getNextCommande()));
    //initialisation
    refreshVar();
    ui->commande->setFocus();
    on_activeClavier_clicked();
    on_taillePile_valueChanged();
    ui->message->setText("Bonjour");
}
//destructeur
MainWindow::~MainWindow()
{
    delete ui;
}
//clavier
void MainWindow::on_button0_clicked(){
    ui->commande->insert("0");
}
void MainWindow::on_button1_clicked(){
    ui->commande->insert("1");
}
void MainWindow::on_button2_clicked(){
    ui->commande->insert("2");
}
void MainWindow::on_button3_clicked(){
    ui->commande->insert("3");
}
void MainWindow::on_button4_clicked(){
    ui->commande->insert("4");
}
void MainWindow::on_button5_clicked(){
    ui->commande->insert("5");
}
void MainWindow::on_button6_clicked(){
    ui->commande->insert("6");
}
void MainWindow::on_button7_clicked(){
    ui->commande->insert("7");
}
void MainWindow::on_button8_clicked(){
    ui->commande->insert("8");
}
void MainWindow::on_button9_clicked(){
    ui->commande->insert("9");
}
void MainWindow::on_buttonPlus_clicked(){
    ui->commande->insert(" +");
    ui->commande->returnPressed();
}
void MainWindow::on_buttonMoins_clicked(){
    ui->commande->insert(" -");
    ui->commande->returnPressed();
}
void MainWindow::on_buttonFois_clicked(){
    ui->commande->insert(" *");
    ui->commande->returnPressed();
}
void MainWindow::on_buttonDiv_clicked(){
    ui->commande->insert(" /");
    ui->commande->returnPressed();
}
void MainWindow::on_buttonPoint_clicked(){
    ui->commande->insert(".");
}
void MainWindow::on_buttonEnter_clicked(){
    ui->commande->returnPressed();
}
void MainWindow::on_activeClavier_clicked(){
    if(ui->activeClavier->isChecked()){
        ui->button0->show();
        ui->button1->show();
        ui->button2->show();
        ui->button3->show();
        ui->button4->show();
        ui->button5->show();
        ui->button6->show();
        ui->button7->show();
        ui->button8->show();
        ui->button9->show();
        ui->buttonPlus->show();
        ui->buttonMoins->show();
        ui->buttonFois->show();
        ui->buttonDiv->show();
        ui->buttonPoint->show();
        ui->buttonEnter->show();
    }
    else{
        ui->button0->hide();
        ui->button1->hide();
        ui->button2->hide();
        ui->button3->hide();
        ui->button4->hide();
        ui->button5->hide();
        ui->button6->hide();
        ui->button7->hide();
        ui->button8->hide();
        ui->button9->hide();
        ui->buttonPlus->hide();
        ui->buttonMoins->hide();
        ui->buttonFois->hide();
        ui->buttonDiv->hide();
        ui->buttonPoint->hide();
        ui->buttonEnter->hide();
    }
}
void MainWindow::on_taillePile_valueChanged(){
    Pile& P = Controleur::getInstance().getPile();
    P.setNbToAffiche((unsigned)ui->afficheTaillePile->text().toInt());
    ui->vuePile->clear();
    ui->vuePile->setRowCount(P.getNbToAffiche());
    QStringList numberlist;
    for(unsigned int i=P.getNbToAffiche();i>0;i--){
            QString str = QString::number(i);
            str+=" : ";
            numberlist<<str;
            ui->vuePile->setItem(i-1,0,new QTableWidgetItem(""));
        }
    ui->vuePile->setVerticalHeaderLabels(numberlist);
    ui->vuePile->setFixedHeight(P.getNbToAffiche()*ui->vuePile->rowHeight(0)+2);
    refreshCalcul();
}
//Variable
void MainWindow::on_modifierVar_clicked(){//pas fonctionelle
    FabriqueLitterale &f=FabriqueLitterale::getInstance();
    ui->message->setText("value changed");
    QHash<QString,LitteraleNumeric*>::iterator It = controleur.Var.begin();
    unsigned int nb=0;
    while(It!=controleur.Var.end()){
        QString id=ui->vueVarId->item(nb,0)->text();
        QString value=ui->vueVarValue->item(nb,0)->text();
        QHash<QString,LitteraleNumeric*>::iterator It2 = controleur.Var.find(id);
        if(value!=It2.value()->toString()){
            LitteraleNumeric *temp =f.fabriquerLitNum(value);
            controleur.Var.erase(It2);
            controleur.Var.insert(id,temp);
        }
        nb++;
        It++;
    }
    refreshVar();
}
void MainWindow::on_supprimerVar_clicked(){
    QList<QTableWidgetItem *> items=ui->vueVarId->selectedItems();
    for(QList<QTableWidgetItem *>::iterator It=items.begin(); It!=items.end();++It)
        controleur.eraseVar((*It)->text());
    refreshVar();
}
void MainWindow::on_toutSupprimerVar_clicked(){
    controleur.Var.clear();
    refreshVar();
}
void MainWindow::refreshVar(){//Mettre a jour de la vueVar
    //Set message
    ui->message->setText("Valeur Enregistrée");
    //Effacer tout
    ui->vueVarId->clear();
    ui->vueVarValue->clear();
    //Mettre a jour de la vueVar
    ui->vueVarId->setRowCount(controleur.Var.count());
    ui->vueVarValue->setRowCount(controleur.Var.count());
    unsigned int nb=0;
    for(QHash<QString,LitteraleNumeric*>::iterator It=controleur.Var.begin(); It!=controleur.Var.end();++It,nb++){
        ui->vueVarId->setItem(nb,0,new QTableWidgetItem(It.key()));
        ui->vueVarValue->setItem(nb,0,new QTableWidgetItem(It.value()->toString()));
    }
    ui->vueVarId->setFixedHeight(5*ui->vueVarId->rowHeight(0)+2);
    ui->vueVarValue->setFixedHeight(5*ui->vueVarValue->rowHeight(0)+2);
}
//Programme
void MainWindow::on_modifierProg_clicked(){

}

void MainWindow::on_supprimerProg_clicked(){

}

void MainWindow::on_toutSupprimerProg_clicked(){
    controleur.Progs.clear();
    refreshProgs();
}
void MainWindow::refreshProgs(){

}

void MainWindow::son(){
    if(ui->activeSon->isChecked()) player->play();
}

void MainWindow::refreshCalcul(){//Mettre a jour de la vuePile
    //Le message
    ui->message->clear();
    //Effacer tout
    Pile& P = controleur.getInstance().getPile();
    for(unsigned int i=0;i<P.getNbToAffiche();i++)
        ui->vuePile->item(i,0)->setText("");
    //Mettre a jour de la vuePile
    unsigned int nb=1;
    for(QVector<Item*>::iterator It=P.itTab.begin(); It!=P.itTab.end() && nb<=P.getNbToAffiche();++It,++nb)
        ui->vuePile->item(P.getNbToAffiche()-nb,0)->setText((*It)->getLitterale().toString());
}

void MainWindow::getNextCommande(){
    QString c = ui->commande->text();
    try{
        controleur.commande(c);
        ui->commande->clear();
    }
    catch (LitteraleException e) { ui->message->setText(e.getInfo());son(); }
    catch (OperateurException o) { ui->message->setText(o.getInfo()); son(); }
    catch (PileException p) { ui->message->setText(p.getInfo()); son(); }
}
