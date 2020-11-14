#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCompleter>
#include <QMainWindow>
#include <QProcess>
#include <QTableWidget>
#include "cn1designerpath.h"
#include "savecn1resfile.h"
#include "editproperty.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void ouvrirProjetViaCmd(const QString &chemin);

private slots:
    void on_choisirDossierSource_clicked();

    void on_scannerDossierSource_clicked();

    void on_actionSave_triggered();

    void on_action_Quit_triggered();

    void on_deleteTranslatedLang_clicked();

    void on_actionSet_CodenameOne_Designer_app_triggered();

    void on_actionSave_in_CodenameOne_Resource_file_triggered();

    void on_addLanguage_clicked();

    void on_actionOpen_triggered();

    void on_actionAbout_triggered();

    void enregistrerResFile(QString fileName);

    void ouvrirFichierRecent(bool arg1);

    void on_translatedLangList_itemSelectionChanged();

    void on_actionAdd_General_Keys_triggered();

    void on_actionAdd_Calendar_Keys_triggered();

    void on_actionAdd_PullToRefresh_Keys_triggered();

    void on_addKey_clicked();

    void on_zoneTraduction_doubleClicked(const QModelIndex &index);

    void on_actionAdd_RTL_Key_triggered();

    void on_key_returnPressed();

    void on_zoneTraduction_itemChanged(QTableWidgetItem *item);

    void on_actionEnable_Disable_Fusion_Theme_triggered();

    void on_viderZoneTraduction_clicked();

private:
    Ui::MainWindow *ui;
    CN1DesignerPath *designerPathDialog;
    SaveCN1ResFile *saveResFile;
    EditProperty *editProperty;

    QString cheminDossierSource;
    QStringList cheminsFichiersSource;
    QHash<QString, QString> countryCode;
    QString fileBaseName;
    QStringList keysCopy;
    QStringList valuesCopy;
    QString FICHIER_TRANSLATE;
    QString cheminFichierRecent;
    QAction *import;
    QAction *exportCSV;
    QAction *exportProperties;
    QAction *exportJSON;

    const QString CODENAMEONE_KEYS_COLOR="#008040";
    const QString OTHERS_KEYS_COLOR="#004080";
    const QString MODIF_NOT_SAVED_COLOR="#d84c4c";
    const QString CN1LINGUIST_GITHUB_PAGE="https://github.com/ericlight/CN1Linguist";

    const QStringList generalKeys={
        "Cancel",
        "OK",
        "Edit",
        "Menu",
        "Select",
        "Save",
        "Done",
    };

    const QStringList pullToRefreshKeys={
        "pull.down",
        "pull.refresh",
        "Updating...",
    };

    const QStringList calendarKeys={
        "Calendar.Jan",
        "Calendar.Feb",
        "Calendar.Mar",
        "Calendar.Apr",
        "Calendar.May",
        "Calendar.Jun",
        "Calendar.Jul",
        "Calendar.Aug",
        "Calendar.Sep",
        "Calendar.Oct",
        "Calendar.Nov",
        "Calendar.Dec",
        "Calendar.Monday",
        "Calendar.Tueday",
        "Calendar.Wednesday",
        "Calendar.Thursday",
        "Calendar.Friday",
        "Calendar.Saturday",
        "Calendar.Sunday",
    };

    void parcourirDossierSource();
    void extraireMotsEtPhrases(QStringList &cheminsFichiers);
    int  extraireNombreElements(const QString &cheminFichier);
    inline void initialiserPaysCountryCode();
    void enregistrerDonnees();
    void ajouterDonnees(bool autoSave);
    inline void appliquerStyles();
    void creerMenuFichierRecents();
    void ajouterCodenameOneKeys(const QStringList &keys);
    void exporterEnCSV();
    void exporterEnProperties();
    void exporterEnJSON();
    void importerSauvegarde();
    void supprimerPropriete();
    void editerCleValeur();
    void dupliquerCleVersValeur();
};

#endif // MAINWINDOW_H
