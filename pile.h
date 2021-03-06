#ifndef Pile_h
#define Pile_h

#include <QMap>
#include <QVector>
#include <QString>
#include <QTextStream>
#include <QHash>



#include "litterale.h"
#include "operateur.h"

using namespace std;

class Operateur;
class ComputerException;
class Item;
class Pile;
class Memento;
class CareTaker;
class Controleur;

/**
   \class ComputerException
   \brief
**/
class ComputerException {
private:
    QString info;
public:
    QString getInfo() const { return info; }
    ComputerException(const QString& s) :info(s) {}
};

/**
   \class Item
   \brief Permet d'encapsuler les litterales
**/
class Item {
private:
    friend class Memento;
    friend class Pile;
    Litterale& L;
    Item(Litterale& l) :L(*l.Clone()) {}
    Item(const Item& It) :L(*It.L.Clone()) {}
    Item& operator=(const Item& i);
public:
    void setLitterale(Litterale& l) { L = *l.Clone(); }
    Litterale& getLitterale() const { return L; }
};

/**
   \class Memento
   \brief Permet Se sauvegarder un état de la Pile
**/
class Memento {
private:
    friend class Controleur;
    friend class CareTaker;
    QVector<Item*> state;
public:
    Memento(const QVector<Item*> items);
    QVector<Item*> getState() const { return state; }
};

/**
   \class Pile
   \brief Permet de stocker les differents Item et de gérer leur priorité (LIFO)
**/
class Pile:public QObject{
private:
    Q_OBJECT
    friend class MainWindow;
    friend class Controleur;
    friend class Xml_Dom;

    QVector<Item*> itTab;
    unsigned int nbAffiche;

    void operator=(const Pile& p);
    Pile(const Pile& p);
public:
    Pile() :nbAffiche(5) { vector<Item*> itTab = *(new vector<Item*>); }
    QVector<Item*> getitTab() const { return itTab; }
    bool estVide() const { return itTab.empty(); }
    void pop();
    Litterale& top() const;
    void push(Litterale& L);
    unsigned int taille() const { return itTab.size(); }
    void setNbToAffiche(unsigned int n) { nbAffiche = n; }
    unsigned int getNbToAffiche() const { return nbAffiche; }
    void setState(const QVector<Item*>& items) { itTab = items; }
    QVector<Item*> getState() { return itTab; }
    void clear() { itTab.clear(); }
signals:
    void modificationEtat();
};

/**
   \class CareTaker
   \brief Permet de conserver memoire les differentes instances de Memento et leur type (Undo ou Redo)
**/
class CareTaker {
private:
    friend class Controleur;
    QVector<Memento*> VecteurUndo;
    QVector<Memento*> VecteurRedo;
    QVector<Litterale*> VecteurLits;
    QString DernierOpUtilise;
    CareTaker() :DernierOpUtilise("") {}
public:
    ~CareTaker();
    QString getDernierOpUtilise() const { return DernierOpUtilise; }
    void setDernierOpUtilise(QString s) { DernierOpUtilise = s; }
    QVector<Litterale*> getVecteurLits() const { return VecteurLits; }
    void clearVecteurLits() { VecteurLits.clear(); }
    void addLitteraleVecteur(Litterale* lit) { VecteurLits.push_back(lit); }
    QVector<Item*> undo();
    QVector<Item*> redo();
};

/**
   \class Controleur
   \brief Permet de gérer l'ahout d'element dans la Pile, et lance les differents operateurs.
**/
class Controleur:public QObject{
    Q_OBJECT
    friend class MainWindow;
    friend class ProgsEdit;
    friend class Expression;
    friend class Xml_Dom;
    friend class Programme;
private:
    Pile& p;
    CareTaker& ct;
    QHash<QString,LitteraleNumeric*> Var;
    QHash<QString,Programme*> Progs;
    Controleur() :p(*new Pile()), ct(*new CareTaker()) {}
    struct Handler {
        Controleur* instance;
        Handler() :instance(nullptr) {}
        ~Handler() { delete instance; }
    };
public:
    static Handler handler;
    static Controleur& getInstance();
    static void libererInstance();
    QHash<QString,LitteraleNumeric*> getVar() const { return Var; }
    void addVar(const QString& s1,LitteraleNumeric* l);
    void eraseVar(const QString& s);
    //LitteraleNumeric* getVar(const QString& s);
    void sauvegardeEtatPile(Operateur* op);
    Pile& getPile() /*const*/ { return p; }
    CareTaker& getCareTaker() const { return ct; }
    void commande(const QString& text);
    void appliquerOperateur(Operateur* Op);
    QHash<QString,Programme*> getProgs() const { return Progs; }
    void addProg(const QString& s1,Programme* l);
    void eraseProg(const QString& s);
    //Programme* getProg(const QString& s);
signals:
    void modification();
};

#endif
