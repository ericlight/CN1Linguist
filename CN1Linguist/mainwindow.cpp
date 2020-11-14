#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDirIterator>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QDomDocument>
#include <QMessageBox>
#include <QToolButton>
#include <QInputDialog>
#include <QDate>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/new/prefix1/images/cn1.png"));

    ui->actionRecent_project_file->setVisible(false);

    ui->zoneTraduction->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->zoneTraduction->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    QToolButton *keysMenu=new QToolButton(this);
    keysMenu->setText(" Codename One Keys");
    keysMenu->setIcon(QIcon(":/new/prefix1/images/menu.png"));
    keysMenu->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    keysMenu->addAction(ui->actionAdd_RTL_Key);
    QAction *separator1=new QAction(this);
    separator1->setSeparator(true);
    keysMenu->addAction(separator1);
    keysMenu->addAction(ui->actionAdd_General_Keys);
    keysMenu->addAction(ui->actionAdd_Calendar_Keys);
    keysMenu->addAction(ui->actionAdd_PullToRefresh_Keys);
    //keysMenu->addAction(ui->actionAdd_All_Keys);

    keysMenu->setPopupMode(QToolButton::InstantPopup);

    QToolButton *exportMenu=new QToolButton(this);
    exportMenu->setText(" Import/Export");
    exportMenu->setIcon(QIcon(":/new/prefix1/images/export2.png"));
    exportMenu->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    import=new QAction("Import...", this);
    QAction *separator2=new QAction(this);
    separator2->setSeparator(true);
    exportCSV=new QAction("Export in CSV format...", this);
    exportProperties=new QAction("Export in Java Properties format...", this);
    exportJSON=new QAction("Export in JSON format...", this);

    connect(import, &QAction::triggered, this, [=](){
        importerSauvegarde();
    });
    connect(exportCSV, &QAction::triggered, this, [=](){
        exporterEnCSV();
    });
    connect(exportProperties, &QAction::triggered, this, [=](){
        exporterEnProperties();
    });
    connect(exportJSON, &QAction::triggered, this, [=](){
        exporterEnJSON();
    });

    exportMenu->addAction(import);
    exportMenu->addAction(separator2);
    exportMenu->addAction(exportCSV);
    exportMenu->addAction(exportProperties);
    exportMenu->addAction(exportJSON);
    exportMenu->setPopupMode(QToolButton::InstantPopup);

    ui->mainToolBar->addWidget(exportMenu);
    ui->mainToolBar->addWidget(keysMenu);

    QAction *deleteKey=new QAction(QIcon(":/new/prefix1/images/close.png"), tr("Remove this property (key and value)"), this);
    QAction *editKeyValue=new QAction(QIcon(":/new/prefix1/images/edit.png"), tr("Edit this property"), this);
    QAction *duplicateKey=new QAction(QIcon(":/new/prefix1/images/copy.png"), tr("Duplicate KEY in the VALUE field"), this);

    connect(deleteKey, &QAction::triggered, this, [=](){
        supprimerPropriete();
    });

    connect(editKeyValue, &QAction::triggered, this, [=](){
        editerCleValeur();
    });

    connect(duplicateKey, &QAction::triggered, this, [=](){
        dupliquerCleVersValeur();
    });

    ui->zoneTraduction->addAction(deleteKey);
    ui->zoneTraduction->addAction(editKeyValue);
    ui->zoneTraduction->addAction(duplicateKey);
    ui->zoneTraduction->setContextMenuPolicy(Qt::ActionsContextMenu);

    ui->zoneTraduction->setSortingEnabled(true);

    QSettings settings("cn1ling-config.ini", QSettings::IniFormat);
    settings.beginGroup("General");
    ui->actionEnable_Disable_Fusion_Theme->setChecked(settings.value("ThemeFusion").toBool());
    settings.endGroup();

    //----------***------------------------------------------------------
    creerMenuFichierRecents();
    appliquerStyles();
    initialiserPaysCountryCode();

    //----------***------------------------------------------------------
    designerPathDialog=new CN1DesignerPath(this);
    saveResFile=new SaveCN1ResFile(this);
    connect(saveResFile, SIGNAL(save(QString)), this, SLOT(enregistrerResFile(QString)));
    editProperty=new EditProperty(this);
    connect(editProperty, &EditProperty::modifierPropriete, this, [=](const QString &prop){
        ui->zoneTraduction->currentItem()->setText(prop);
    });   
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ouvrirProjetViaCmd(const QString &chemin)
{
    cheminDossierSource=chemin;
    if(!cheminDossierSource.endsWith("src")){
        QMessageBox::warning(this,tr("SRC folder missed!"),tr("You have to choose a Codename One project folder"));
        return;
    }

    if(ui->translatedLangList->count()>0){
        ui->translatedLangList->clear();
    }
    if(!ui->bundleName->text().isEmpty()){
         ui->bundleName->clear();
    }
    if(ui->zoneTraduction->rowCount()>0){
        ui->zoneTraduction->clearContents();
        ui->zoneTraduction->setRowCount(0);
    }

    ui->currentLang->setText("-");

    QFileInfo fileInfo(cheminDossierSource);

    QString projectFolderPath=fileInfo.canonicalPath();
    QString l10nPathFolder=projectFolderPath+QDir::separator()+"l10n";
    QDir l10nFolder(l10nPathFolder);
    if(!l10nFolder.exists()){
        l10nFolder.mkdir(l10nPathFolder);
    }
    QString projectFolderName=QFileInfo(projectFolderPath).baseName();
    FICHIER_TRANSLATE=l10nPathFolder+QDir::separator()+projectFolderName+".cn1l";

    if(!cheminDossierSource.isEmpty()){
        ui->cheminDossierSource->setText(cheminDossierSource);
    }
}

inline void MainWindow::initialiserPaysCountryCode()
{
    countryCode.insert("English (en)","en");
    countryCode.insert("Spanish (es)","es");
    countryCode.insert("Portuguese (pt)","pt");
    countryCode.insert("French (fr)","fr");
    countryCode.insert("German (de)","de");
    countryCode.insert("Chinese (zh)","zh");
    countryCode.insert("Afar (aa)","aa");
    countryCode.insert("Abkhazian (ab)","ab");
    countryCode.insert("Avestan (ae)","ae");
    countryCode.insert("Afrikaans (af)","af");
    countryCode.insert("Akan (ak)","ak");
    countryCode.insert("Amharic (am)","am");
    countryCode.insert("Aragonese (an)","an");
    countryCode.insert("Arabic (ar)","ar");
    countryCode.insert("Assamese (as)","as");
    countryCode.insert("Avaric (av)","av");
    countryCode.insert("Aymara (ay)","ay");
    countryCode.insert("Azerbaijani (az)","az");
    countryCode.insert("Bashkir (ba)","ba");
    countryCode.insert("Belarusian (be)","be");

    countryCode.insert("Bulgarian (bg)","bg");
    countryCode.insert("Bihari (bh)","bh");
    countryCode.insert("Bislama (bi)","bi");
    countryCode.insert("Bambara (bm)","bm");
    countryCode.insert("Bengali (bn)","bn");
    countryCode.insert("Tibetan (bo)","bo");
    countryCode.insert("Breton (br)","br");
    countryCode.insert("Bosnian (bs)","bs");
    countryCode.insert("Catalan (ca)","ca");
    countryCode.insert("Chechen (ce)","ce");
    countryCode.insert("Chamorro (ch)","ch");
    countryCode.insert("Corsican (co)","co");
    countryCode.insert("Cree (cr)","cr");
    countryCode.insert("Czech (cs)","cs");
    countryCode.insert("Old Church Slavonic (cu)","cu");
    countryCode.insert("Chuvash (cv)","cv");
    countryCode.insert("Welsh (cy)","cy");
    countryCode.insert("Danish (da)","da");
    countryCode.insert("Divehi (dv)","dv");
    countryCode.insert("Dzongkha (dz)","dz");

    countryCode.insert("Ewe (ee)","ee");
    countryCode.insert("Greek (el)","el");
    countryCode.insert("Esperanto (eo)","eo");
    countryCode.insert("Estonian (et)","et");
    countryCode.insert("Basque (eu)","eu");
    countryCode.insert("Persian (fa)","fa");
    countryCode.insert("Fulah (ff)","ff");
    countryCode.insert("Finnish (fi)","fi");
    countryCode.insert("Fijian (fj)","fj");
    countryCode.insert("Faroese (fo)","fo");
    countryCode.insert("Western Frisian (fy)","fy");
    countryCode.insert("Irish (ga)","ga");
    countryCode.insert("Scottish Gaelic (gd)","gd");
    countryCode.insert("Galician (gl)","gl");
    countryCode.insert("Guarani (gn)","gn");
    countryCode.insert("Gujarati (gu)","gu");
    countryCode.insert("Manx (gv)","gv");
    countryCode.insert("Hausa (ha)","ha");
    countryCode.insert("Hebrew (he)","he");
    countryCode.insert("Hindi (hi)","hi");

    countryCode.insert("Hiri Motu (ho)","ho");
    countryCode.insert("Croatian (hr)","hr");
    countryCode.insert("Haitian (ht)","ht");
    countryCode.insert("Hungarian (hu)","hu");
    countryCode.insert("Armenian (hy)","hy");
    countryCode.insert("Herero (hz)","hz");
    countryCode.insert("Interlingua (ia)","ia");
    countryCode.insert("Indonesian (id)","id");
    countryCode.insert("Interlingue (ie)","ie");
    countryCode.insert("Igbo (ig)","ig");
    countryCode.insert("Sichuan Yi (ii)","ii");
    countryCode.insert("Inupiaq (ik)","ik");
    countryCode.insert("Ido (io)","io");
    countryCode.insert("Icelandic (is)","is");
    countryCode.insert("Italian (it)","it");
    countryCode.insert("Inuktitut (iu)","iu");
    countryCode.insert("Japanese (ja)","ja");
    countryCode.insert("Javanese (jv)","jv");
    countryCode.insert("Georgian (ka)","ka");
    countryCode.insert("Kongo (kg)","kg");

    countryCode.insert("Kikuyu (ki)","ki");
    countryCode.insert("Kwanyama (kj)","kj");
    countryCode.insert("Kazakh (kk)","kk");
    countryCode.insert("Kalaallisut (kl)","kl");
    countryCode.insert("Khmer (km)","km");
    countryCode.insert("Kannada (kn)","kn");
    countryCode.insert("Korean (ko)","ko");
    countryCode.insert("Kanuri (kr)","kr");
    countryCode.insert("Kashmiri (ks)","ks");
    countryCode.insert("Kurdish (ku)","ku");
    countryCode.insert("Komi (kv)","kv");
    countryCode.insert("Cornish (kw)","kw");
    countryCode.insert("Kirghiz (ky)","ky");
    countryCode.insert("Latin (la)","la");
    countryCode.insert("Luxembourgish (lb)","lb");
    countryCode.insert("Ganda (lg)","lg");
    countryCode.insert("Limburgish (li)","li");
    countryCode.insert("Lingala (ln)","ln");
    countryCode.insert("Lao (lo)","lo");
    countryCode.insert("Lithuanian (lt)","lt");

    countryCode.insert("Luba-Katanga (lu)","lu");
    countryCode.insert("Latvian (lv)","lv");
    countryCode.insert("Malagasy (mg)","mg");
    countryCode.insert("Marshallese (mh)","mh");
    countryCode.insert("Maori (mi)","mi");
    countryCode.insert("Macedonian (mk)","mk");
    countryCode.insert("Malayalam (ml)","ml");
    countryCode.insert("Mongolian (mn)","mn");
    countryCode.insert("Marathi (mr)","mr");
    countryCode.insert("Malay (ms)","ms");
    countryCode.insert("Maltese (mt)","mt");
    countryCode.insert("Burmese (my)","my");
    countryCode.insert("Nauru (na)","na");
    countryCode.insert("Norwegian Bokmal (nb)","nb");
    countryCode.insert("North Ndebele (nd)","nd");
    countryCode.insert("Nepali (ne)","ne");
    countryCode.insert("Ndonga (ng)","ng");
    countryCode.insert("Dutch (nl)","nl");
    countryCode.insert("Norwegian Nynorsk (nn)","nn");
    countryCode.insert("Norwegian (no)","no");

    countryCode.insert("South Ndebele (nr)","nr");
    countryCode.insert("Navajo (nv)","nv");
    countryCode.insert("Chichewa (ny)","ny");
    countryCode.insert("Occitan (oc)","oc");
    countryCode.insert("Ojibwa (oj)","oj");
    countryCode.insert("Oromo (om)","om");
    countryCode.insert("Oriya (or)","or");
    countryCode.insert("Ossetian (os)","os");
    countryCode.insert("Panjabi (pa)","pa");
    countryCode.insert("Pali (pi)","pi");
    countryCode.insert("Polish (pl)","pl");
    countryCode.insert("Pashto (ps)","ps");
    countryCode.insert("Quechua (qu)","qu");
    countryCode.insert("Reunionese (rc)","rc");
    countryCode.insert("Romansh (rm)","rm");
    countryCode.insert("Kirundi (rn)","rn");
    countryCode.insert("Romanian (ro)","ro");
    countryCode.insert("Russian (ru)","ru");
    countryCode.insert("Kinyarwanda (rw)","rw");
    countryCode.insert("Sanskrit (sa)","sa");

    countryCode.insert("Sardinian (sc)","sc");
    countryCode.insert("Sindhi (sd)","sd");
    countryCode.insert("Northern Sami (se)","se");
    countryCode.insert("Sango (sg)","sg");
    countryCode.insert("Serbo-Croatian (sh)","sh");
    countryCode.insert("Sinhalese (si)","si");
    countryCode.insert("Slovak (sk)","sk");
    countryCode.insert("Slovenian (sl)","sl");
    countryCode.insert("Samoan (sm)","sm");
    countryCode.insert("Shona (sn)","sn");
    countryCode.insert("Somali (so)","so");
    countryCode.insert("Albanian (sq)","sq");
    countryCode.insert("Serbian (sr)","sr");
    countryCode.insert("Swati (ss)","ss");
    countryCode.insert("Sotho (st)","st");
    countryCode.insert("Sundanese (su)","su");
    countryCode.insert("Swedish (sv)","sv");
    countryCode.insert("Swahili (sw)","sw");
    countryCode.insert("Tamil (ta)","ta");
    countryCode.insert("Telugu (te)","te");

    countryCode.insert("Tajik (tg)","tg");
    countryCode.insert("Thai (th)","th");
    countryCode.insert("Tigrinya (ti)","ti");
    countryCode.insert("Turkmen (tk)","tk");
    countryCode.insert("Tagalog (tl)","tl");
    countryCode.insert("Tswana (tn)","tn");
    countryCode.insert("Tonga (to)","to");
    countryCode.insert("Turkish (tr)","tr");
    countryCode.insert("Tsonga (ts)","ts");
    countryCode.insert("Tatar (tt)","tt");
    countryCode.insert("Twi (tw)","tw");
    countryCode.insert("Tahitian (ty)","ty");
    countryCode.insert("Uighur (ug)","ug");
    countryCode.insert("Ukrainian (uk)","uk");
    countryCode.insert("Urdu (ur)","ur");
    countryCode.insert("Uzbek (uz)","uz");
    countryCode.insert("Venda (ve)","ve");
    countryCode.insert("Viet Namese (vi)","vi");
    countryCode.insert("Volapuk (vo)","vo");
    countryCode.insert("Walloon (wa)","wa");

    countryCode.insert("Wolof (wo)","wo");
    countryCode.insert("Xhosa (xh)","xh");
    countryCode.insert("Yiddish (yi)","yi");
    countryCode.insert("Yoruba (yo)","yo");
    countryCode.insert("Zhuang (za)","za");
    countryCode.insert("Zulu (zu)","zu");
}

void MainWindow::on_choisirDossierSource_clicked()
{
    QString cheminDossierProjet=QFileDialog::getExistingDirectory(this,tr("Choose the project folder"),".");
    cheminDossierSource=cheminDossierProjet+"/src";
    QDir dossier(cheminDossierSource);
    if(!dossier.exists()){
        QMessageBox::warning(this,tr("SRC folder missed in this folder!"),tr("You have to choose a <b>Codename One project folder</b> (which has an SRC folder)"));
        return;
    }

    if(ui->translatedLangList->count()>0){
        ui->translatedLangList->clear();
    }
    if(!ui->bundleName->text().isEmpty()){
         ui->bundleName->clear();
    }
    if(ui->zoneTraduction->rowCount()>0){
        ui->zoneTraduction->clearContents();
        ui->zoneTraduction->setRowCount(0);
    }

    ui->currentLang->setText("-");

    QFileInfo fileInfo(cheminDossierSource);

    QString projectFolderPath=fileInfo.canonicalPath();
    QString l10nPathFolder=projectFolderPath+QDir::separator()+"l10n";
    QDir l10nFolder(l10nPathFolder);

    if(!l10nFolder.exists()){
        l10nFolder.mkdir(l10nPathFolder);
    }
    QString projectFolderName=QFileInfo(projectFolderPath).baseName();
    FICHIER_TRANSLATE=l10nPathFolder+QDir::separator()+projectFolderName+".cn1l";

    if(!cheminDossierSource.isEmpty()){
        ui->cheminDossierSource->setText(cheminDossierProjet);
    }
}

void MainWindow::parcourirDossierSource()
{
    QDirIterator dIt(ui->cheminDossierSource->text(),QDir::Files|QDir::NoSymLinks,QDirIterator::Subdirectories);
    cheminsFichiersSource.clear();

    while(dIt.hasNext()){
        const QString cheminFichierSource=dIt.next();
        if(cheminFichierSource.endsWith(".java")){
            cheminsFichiersSource<<cheminFichierSource;
        }
    }
}

void MainWindow::extraireMotsEtPhrases(QStringList &cheminsFichiers)
{
    QStringList listeMotsEtPhrases;
    QFile file;

    //----------Pour extraire les mots entourés de tr()---------------------
    foreach(QString cheminFichier, cheminsFichiers){

        file.setFileName(cheminFichier);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream stream(&file);
        stream.setCodec("UTF-8");

        while (!stream.atEnd()) {
          QString line = file.readLine();
          int pos1=0;

          int position1=line.indexOf("tr");
          if(position1!=-1){
              pos1=position1;
          }

          int position2=line.indexOf("\")");
          if(position2!=-1){
            int nbreDeCaract=position2-pos1;

            QString content=line.mid(pos1+4,nbreDeCaract-4);
            if(!content.isEmpty()){
                if(line.contains("tr")){
                    listeMotsEtPhrases<<content;
                }
            }
          }

        } //Fin while--------------------------
        file.close();
    }
    //----------------------------
    ui->zoneTraduction->clearContents();
    ui->zoneTraduction->setRowCount(0);

    foreach (QString motOuPhrase, listeMotsEtPhrases) {
        QTableWidgetItem *itemKey=new QTableWidgetItem(motOuPhrase);
        QTableWidgetItem *itemValue=new QTableWidgetItem("");

        //itemKey->setFlags(Qt::NoItemFlags);
        itemKey->setForeground(QColor(OTHERS_KEYS_COLOR));
        itemKey->setFont(QFont(itemKey->font().family(),10,QFont::Bold));

        int ligne = ui->zoneTraduction->rowCount();
        ui->zoneTraduction->insertRow(ligne);

        ui->zoneTraduction->setItem(ligne,0,itemKey);
        ui->zoneTraduction->setItem(ligne,1,itemValue);
    }

    listeMotsEtPhrases.clear();
}

int MainWindow::extraireNombreElements(const QString &cheminFichier)
{
    QStringList listeMotsEtPhrases;

    QFile file(cheminFichier);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream stream(&file);


    while (!stream.atEnd()) {
      QString line = file.readLine();
      int pos1 = 0;

      int position1=line.indexOf("CL.tr");
      if(position1!=-1){
          pos1=position1;
      }

      int position2=line.indexOf("\")");
      if(position2!=-1){
        int nbreDeCaract=position2-pos1;

        QString content=line.mid(pos1+7,nbreDeCaract-7);
        if(!content.isEmpty()){
            if(line.contains("CL.tr")){
                listeMotsEtPhrases<<content;
            }
        }
      }
    }

    int contentsSize=listeMotsEtPhrases.size();
    listeMotsEtPhrases.clear();
    file.close();

    return contentsSize;
}

void MainWindow::on_scannerDossierSource_clicked()
{
    if(ui->cheminDossierSource->text().isEmpty()){
        QMessageBox::warning(this, tr("Project missed"), tr("You have to select a <b>Codename One project</b> folder before"));
        return;
    }

    //---------------------
    if(FICHIER_TRANSLATE.isEmpty()){
        QFileInfo fileInfo(ui->cheminDossierSource->text());
        QString projectFolderPath=fileInfo.canonicalPath();
        QString l10nPathFolder=projectFolderPath+QDir::separator()+"l10n";
        QDir l10nFolder(l10nPathFolder);

        if(!l10nFolder.exists()){
            l10nFolder.mkdir(l10nPathFolder);
        }
        QString projectFolderName=QFileInfo(projectFolderPath).baseName();
        FICHIER_TRANSLATE=l10nPathFolder+QDir::separator()+projectFolderName+".cn1l";
    }
    //------------------------

    ui->scannerDossierSource->setEnabled(false);
    //NOTE: Mettre après l'exécution de la méthode "parcourirDossierSource()" dans un thread pour éviter que l'interface ne se fige pendant de longues opérations de scan
    parcourirDossierSource();
    ui->scannerDossierSource->setEnabled(true);

    extraireMotsEtPhrases(cheminsFichiersSource);
    //--------------------------

    ui->zoneTraduction->horizontalHeaderItem(0)->setText(QString("KEY - %1 items").arg(ui->zoneTraduction->rowCount()));
}

void MainWindow::on_actionSave_triggered()
{
    if(ui->translatedLangList->selectedItems().size()==0){
        QMessageBox::warning(this, tr("Language missed"), tr("You have to select a language before"));
        return;
    }

    QString cheminProjet=ui->cheminDossierSource->text();
    if(cheminProjet.isEmpty()){
        QMessageBox::warning(this,tr("Project folder missed"),tr("You have to choose a <b>Codename One project folder</b> before"));
        return;
    }

    QString l10nPathFolder=cheminProjet+QDir::separator()+"l10n";
    QDir l10nFolder(l10nPathFolder);
    if(!l10nFolder.exists()){
        l10nFolder.mkdir(l10nPathFolder);
        QString projectFolderName=QFileInfo(cheminProjet).baseName();
        FICHIER_TRANSLATE=l10nPathFolder+QDir::separator()+projectFolderName+".cn1l";
    }

    if(QFile::exists(FICHIER_TRANSLATE)){
        ajouterDonnees(false);
    } else {
        enregistrerDonnees();
    }

    statusBar()->showMessage("Modification saved", 3000);
}

void MainWindow::on_action_Quit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_deleteTranslatedLang_clicked()
{
    if(ui->translatedLangList->count()==0){
        QMessageBox::warning(this,tr("Delete"),tr("There is no language to remove."));
        return;
    }

    QListWidgetItem *selectedItem=ui->translatedLangList->currentItem();
    if(selectedItem==nullptr){
        QMessageBox::warning(this,tr("Delete"),tr("You have to select the language that you want to delete."));
        return;
    }

    int reponse=QMessageBox::question(this,tr("Confirm"),tr("Do you really want to remove this language ?"), QMessageBox::Yes|QMessageBox::No);
    if(reponse==QMessageBox::Yes){
        ui->currentLang->setText("-");

        QDomDocument dom;

        QFile xml_doc(FICHIER_TRANSLATE);
        if(xml_doc.exists()){
            if(!xml_doc.open(QIODevice::ReadWrite))
            {
                //QMessageBox::warning(this, "Erreur à l'ouverture du document", "Le document <b>grille.xml</b> n'a pas pu être ouvert.");
                return;
            }

            if(!dom.setContent(&xml_doc))
            {
                 xml_doc.close();
                 //QMessageBox::warning(this, "Erreur à l'ouverture du document XML", "Le document <b>grille.xml</b> n'a pas pu être attribué à l'objet QDomDocument.");
                 return;
            }
        }
        xml_doc.close();

        QString codePays=countryCode.value(selectedItem->text());

        QDomElement l10n_element=dom.documentElement();
        QDomNodeList langNodesList=l10n_element.childNodes();
        int langNbreNode=langNodesList.count();

        QListWidgetItem *item_retire=ui->translatedLangList->takeItem(ui->translatedLangList->currentRow());
        delete item_retire;

        for(int i=0;i<langNbreNode;i++) {
            QDomNode langNode=langNodesList.at(i);
            QDomElement elementLang=langNode.toElement();

            if(elementLang.attribute("name")==codePays){
                l10n_element.removeChild(langNode);

                QString xml = dom.toString(3);
                xml_doc.setFileName(FICHIER_TRANSLATE);
                xml_doc.open(QIODevice::WriteOnly);
                QTextStream stream(&xml_doc);
                stream.setCodec("UTF-8");
                stream<<xml;
                xml_doc.close();
            }
        }
    }
}

void MainWindow::on_actionSet_CodenameOne_Designer_app_triggered()
{
    designerPathDialog->show();
    designerPathDialog->raise();
    designerPathDialog->activateWindow();
}

void MainWindow::on_actionSave_in_CodenameOne_Resource_file_triggered()
{
    QString resFilePath=ui->cheminDossierSource->text().trimmed()+"/src/theme.res";
    saveResFile->setResFilePath(resFilePath);

    saveResFile->show();
    saveResFile->raise();
    saveResFile->activateWindow();
}

void MainWindow::enregistrerDonnees()
{
    QString bundleName=ui->bundleName->text().trimmed();
    if(bundleName.isEmpty()){
        QMessageBox::warning(this,tr("Bundle name missed!"),tr("You have to enter the <b>translation bundle name</b> before saving"));
        return;
    }

    QDomDocument doc;

    QDomElement l10n = doc.createElement("l10n");
    l10n.setAttribute("name",bundleName);
    doc.appendChild(l10n);

    int count=ui->zoneTraduction->rowCount();

    QDomElement lang = doc.createElement("lang");
    lang.setAttribute("name",countryCode.value(ui->translatedLangList->currentItem()->text()));

    for(int i=0;i<count;i++){
        QTableWidgetItem *itemKey=ui->zoneTraduction->item(i,0);
        QTableWidgetItem *itemValue=ui->zoneTraduction->item(i,1);

        QDomElement entry = doc.createElement("entry");
        entry.setAttribute("key",itemKey->text().trimmed());
        entry.setAttribute("value",itemValue->text().trimmed());

        lang.appendChild(entry);
    }

    l10n.appendChild(lang);

    QString xml = doc.toString(3);
    QString versionXML="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml=versionXML+xml;

    QFile fichier(FICHIER_TRANSLATE);
    fichier.open(QIODevice::WriteOnly);
    QTextStream stream(&fichier);
    stream.setCodec("UTF-8");
    stream<<xml;
    fichier.close();

    if(fichier.flush()){
        QMessageBox::information(this,tr("Done!"),tr("Contents saved successfully in <b>%1</b>!").arg(FICHIER_TRANSLATE));
    } else {
        QMessageBox::critical(this,tr("Failed!"),tr("Failed to save contents"));
    }
}

void MainWindow::ajouterDonnees(bool autoSave)
{
    QDomDocument dom;

    QFile xml_doc(FICHIER_TRANSLATE);
    if(xml_doc.exists()){
        if(!xml_doc.open(QIODevice::ReadOnly))
        {
            //QMessageBox::warning(this, "Erreur à l'ouverture du document", "Le document <b>grille.xml</b> n'a pas pu être ouvert.");
            return;
        }

        if(!dom.setContent(&xml_doc))
        {
             xml_doc.close();
//             QMessageBox::warning(this, "Erreur à l'ouverture du document XML", "Le document <b>grille.xml</b> n'a pas pu être attribué à l'objet QDomDocument.");
             return;
        }
    }
    xml_doc.close();


    QString bundleName=ui->bundleName->text().trimmed();
    if(autoSave==false){
        if(bundleName.isEmpty()){
            QMessageBox::warning(this,tr("Bundle name missed!"),tr("You have to enter the <b>translation bundle name</b> before saving"));
            return;
        }
    }

    QDomElement l10n_element=dom.documentElement();
    l10n_element.setAttribute("name",bundleName);

    //-----------Si les données de cette langue existent déjà, alors il faut les supprimer avant d'ajouter à nouveau les infos modifiées------------
    QListWidgetItem *selectedItem=ui->translatedLangList->currentItem();
    if(selectedItem==nullptr){
        QMessageBox::warning(this,tr("Language selection missed"),tr("You have to <b>select the language</b> that you want to save before"));
        return;
    }
    QString codePays=countryCode.value(selectedItem->text());
    QDomNodeList langNodesList=l10n_element.childNodes();
    int langNbreNode=langNodesList.count();

    for(int i=0;i<langNbreNode;i++) {
        QDomNode langNode=langNodesList.at(i);
        QDomElement elementLang=langNode.toElement();

        if(elementLang.attribute("name")==codePays){
            l10n_element.removeChild(langNode);
        }
    }

//---------------------------------------------------------------------------------------------------------
    int count=ui->zoneTraduction->rowCount();
    QDomElement lang = dom.createElement("lang");
    lang.setAttribute("name",codePays);

    for(int i=0;i<count;i++){
        QTableWidgetItem *itemKey=ui->zoneTraduction->item(i,0);
        QTableWidgetItem *itemValue=ui->zoneTraduction->item(i,1);

        QDomElement entry = dom.createElement("entry");
        entry.setAttribute("key",itemKey->text().trimmed());
        entry.setAttribute("value",itemValue->text().trimmed());

        lang.appendChild(entry);
    }

    l10n_element.appendChild(lang);

    QString xml = dom.toString(3);

    xml_doc.setFileName(FICHIER_TRANSLATE);
    xml_doc.open(QIODevice::WriteOnly);
    QTextStream stream(&xml_doc);
    stream.setCodec("UTF-8");
    stream<<xml;
    xml_doc.close();
}

void MainWindow::on_addLanguage_clicked()
{
    if(ui->countryCode->currentIndex()!=0){

//        if(ui->zoneTraduction->rowCount()==0){
//            QMessageBox::warning(this, tr("Contents missed!"), tr("You have to scan your project before"));
//            return;
//        }

        int count=ui->translatedLangList->count();
        for(int i=0;i<count;++i){
            QListWidgetItem *item=ui->translatedLangList->item(i);
            if(ui->countryCode->currentText()==item->text()){
                QMessageBox::warning(this, tr("Already added !!"), tr("This language is already added !"));
                return;
            }
        }
        QListWidgetItem *itemLang=new QListWidgetItem(ui->countryCode->currentText());
        itemLang->setIcon(QIcon(":/new/prefix1/images/point.png"));

        ui->translatedLangList->addItem(itemLang);
    }
}

inline void MainWindow::appliquerStyles()
{
    QFile style(":/new/prefix1/styles.css");
    style.open(QIODevice::ReadOnly);
    QTextStream styleStream(&style);
    QString contenuStyle=styleStream.readAll();
    style.close();

    ui->cheminDossierSource->setStyleSheet(contenuStyle);
    ui->bundleName->setStyleSheet(contenuStyle);
    ui->key->setStyleSheet(contenuStyle);
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName=QFileDialog::getOpenFileName(this,"Choose Codename One Linguist File",".","CN1L file (*.cn1l)");
    if(fileName.isEmpty()){
        return;
    }

    QDomDocument dom;
    QFile xml_doc(fileName);
    if(xml_doc.exists()){
        if(!xml_doc.open(QIODevice::ReadOnly))
        {
            //QMessageBox::warning(this, "Erreur à l'ouverture du document", "Le document <b>grille.xml</b> n'a pas pu être ouvert.");
            return;
        }

        if(!dom.setContent(&xml_doc))
        {
             xml_doc.close();
             //QMessageBox::warning(this, "Erreur à l'ouverture du document XML", "Le document <b>grille.xml</b> n'a pas pu être attribué à l'objet QDomDocument.");
             return;
        }
    }
    xml_doc.close();

    QDomElement l10n_element=dom.documentElement();
    ui->bundleName->setText(l10n_element.attribute("name"));

    QDomNodeList langNodesList=l10n_element.childNodes();
    int langNbreNode=langNodesList.count();

    ui->translatedLangList->clear();

    for(int i=0;i<langNbreNode;i++) {
        QDomNode langNode=langNodesList.at(i);
        QDomElement elementLang=langNode.toElement();

        QString langName=elementLang.attribute("name");

        QListWidgetItem *itemLang=new QListWidgetItem(countryCode.key(langName));
        itemLang->setIcon(QIcon(":/new/prefix1/images/point.png"));

        ui->translatedLangList->addItem(itemLang);

        //---Nouveau code---------------------------------------
        QDomNodeList entryNodesList=elementLang.childNodes();
        int entryNbreNode=entryNodesList.count();

        ui->zoneTraduction->clearContents();
        ui->zoneTraduction->setRowCount(0);

        for(int i=0;i<entryNbreNode;i++) {
            QDomNode entryNode=entryNodesList.at(i);
            QDomElement elementEntry=entryNode.toElement();

            QString key=elementEntry.attribute("key");
            QTableWidgetItem *itemKey=new QTableWidgetItem(key);
            QTableWidgetItem *itemValue=new QTableWidgetItem("");

            if(generalKeys.contains(key) || pullToRefreshKeys.contains(key) || calendarKeys.contains(key)){
                itemKey->setForeground(QColor(CODENAMEONE_KEYS_COLOR));
            } else {
                itemKey->setForeground(QColor(OTHERS_KEYS_COLOR));
            }

            //itemKey->setFlags(Qt::NoItemFlags);
            itemKey->setFont(QFont(itemKey->font().family(),10,QFont::Bold));


            int ligne = ui->zoneTraduction->rowCount();
            ui->zoneTraduction->insertRow(ligne);

            ui->zoneTraduction->setItem(ligne,0,itemKey);
            ui->zoneTraduction->setItem(ligne,1,itemValue);
        }
        //---Fin nouveau code-----------------------------------
    }

    ui->zoneTraduction->horizontalHeaderItem(0)->setText(QString("KEY - %1 items").arg(ui->zoneTraduction->rowCount()));

    QFileInfo fileInfo(fileName);
    QString projectFolderPath=fileInfo.canonicalPath();
    QString srcPathFolder=QFileInfo(projectFolderPath).canonicalPath()+"/src";

    QDir srcFolder(srcPathFolder);
    if(srcFolder.exists()){
        FICHIER_TRANSLATE=fileName;
        ui->cheminDossierSource->setText(QFileInfo(projectFolderPath).canonicalPath());
    }

    QSettings settings("cn1ling-config.ini", QSettings::IniFormat);
    settings.beginGroup("RecentsFiles");
    settings.setValue("recents_files",fileName);
    settings.endGroup();

    creerMenuFichierRecents();

}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this,tr("About Codename One Linguist"), tr("<b>Codename One Linguist</b> is a desktop tool which simplify internationalization and localization for Codename One mobile apps.<br/><br/>"
                                                           "Built by <b>Eric Dodji Gbofu</b> for Codename One developers community<br/><br/>"
                                                           "<u>GitHub :</u> <a href='"+CN1LINGUIST_GITHUB_PAGE.toLocal8Bit()+"'><u>"+CN1LINGUIST_GITHUB_PAGE.toLocal8Bit()+"</u></a>"));
}

void MainWindow::enregistrerResFile(QString fileName)
{
    QFileInfo fileInfo(fileName);
    QString nomFichierSansExtension=fileInfo.baseName();

    QString srcFolderPath=fileInfo.canonicalPath();
    QString projectFolderPath=QFileInfo(srcFolderPath).canonicalPath();

    QString resFolderPath=projectFolderPath+"/res";
    QDir resFolder(resFolderPath);

    if(!resFolder.exists()){
        QMessageBox::warning(this,tr("RES folder missing!"), tr("The RES folder is missing in your path."));
        return;
    }

    QString themeXMLFilePath=resFolderPath+"/"+nomFichierSansExtension+".xml";

    //------Récupération du DOM du fichier XML du dossier RES------------------
    QDomDocument xmltheme_dom;
    QFile xmlFile(themeXMLFilePath);
    if(xmlFile.exists()){
        if(!xmlFile.open(QIODevice::ReadOnly))
        {
            //QMessageBox::warning(this, "Erreur à l'ouverture du document", "Le document <b>grille.xml</b> n'a pas pu être ouvert.");
            return;
        }

        if(!xmltheme_dom.setContent(&xmlFile))
        {
             xmlFile.close();
             //QMessageBox::warning(this, "Erreur à l'ouverture du document XML", "Le document <b>grille.xml</b> n'a pas pu être attribué à l'objet QDomDocument.");
             return;
        }
    }
    xmlFile.close();

    QDomElement resource_element=xmltheme_dom.documentElement();

    //------Récupération du DOM du fichier XML contenant les textes de traduction---------
    QString l10nPathFolder=projectFolderPath+"/l10n";
    QString projectFolderName=QFileInfo(projectFolderPath).baseName();
    QString l10nXmlFilePath=l10nPathFolder+"/"+projectFolderName+".cn1l";

    QDomDocument l10nxml_dom;
    xmlFile.setFileName(l10nXmlFilePath);
    if(xmlFile.exists()){
        if(!xmlFile.open(QIODevice::ReadOnly))
        {
            //QMessageBox::warning(this, "Erreur à l'ouverture du document", "Le document <b>grille.xml</b> n'a pas pu être ouvert.");
            return;
        }

        if(!l10nxml_dom.setContent(&xmlFile))
        {
             xmlFile.close();
             //QMessageBox::warning(this, "Erreur à l'ouverture du document XML", "Le document <b>grille.xml</b> n'a pas pu être attribué à l'objet QDomDocument.");
             return;
        }
    }
    xmlFile.close();

    QDomElement l10n_element=l10nxml_dom.documentElement();

    //----------------Suppression des données du contenu du fichier theme.XML s'il y étaient déjà------
    QDomNodeList l10nNodesList=resource_element.elementsByTagName("l10n");
    int nbreNode=l10nNodesList.count();
    for(int i=0;i<nbreNode;i++) {
        QDomNode node=l10nNodesList.at(i);
        QDomElement element=node.toElement();

        if(element.attribute("name")==ui->bundleName->text().trimmed()){
            resource_element.removeChild(node);
        }
    }
    //----------------Ajout du contenu du fichier CN1L de traduction au fichier theme.XML---------------------------
    resource_element.appendChild(l10n_element);

    QString xml = xmltheme_dom.toString(3);

    xmlFile.setFileName(themeXMLFilePath);
    xmlFile.open(QIODevice::WriteOnly);
    QTextStream stream(&xmlFile);
    stream.setCodec("UTF-8");
    stream<<xml;

    xmlFile.close();

    //-----------------Génération du fichier RES via le fichier XML-------------------------
    QSettings parametres("cn1ling-config.ini", QSettings::IniFormat);
    parametres.beginGroup("General");
    QString designer_path=parametres.value("cn1designer_path").toString();
    QString java_path=parametres.value("java_path").toString();
    parametres.endGroup();

    if(designer_path.isEmpty()){
        QMessageBox::warning(this, tr("Codename One Designer location missed"), tr("Please, choose the <b>Codename One Designer path</b> in the <b>Settings</b> menu"));
        return;
    }

    if(java_path.isEmpty()){
        QMessageBox::warning(this, tr("Java JDK location missed"), tr("Please, choose the <b>Java JDK</b> location in the <b>Settings</b> menu"));
        return;
    }

    QString java_exe_path=java_path+"/bin/java";

    QStringList arguments;
    arguments<<"-jar";
    arguments<<designer_path;
    arguments<<"-sync-xml";
    arguments<<fileName;

    bool success=QProcess::startDetached(java_exe_path, arguments);

    if(success==true){
        QMessageBox::information(this,tr("Done!"),tr("Languages and translated words are successfully added to the <b>%1</b> resource file").arg(fileInfo.fileName()));
    } else {
        QMessageBox::critical(this,tr("Failed!"),tr("Failed to add languages and translated words to the project resource file"));
    }
}

void MainWindow::creerMenuFichierRecents()
{
    QSettings settings("cn1ling-config.ini", QSettings::IniFormat);
    settings.beginGroup("RecentsFiles");

    QString recent_file=settings.value("recents_files").toString();

    if(!recent_file.isEmpty()){
        cheminFichierRecent=recent_file;

        ui->actionRecent_project_file->setText("Recent file : "+recent_file);
        ui->actionRecent_project_file->setVisible(true);
        connect(ui->actionRecent_project_file, SIGNAL(triggered(bool)), this, SLOT(ouvrirFichierRecent(bool)));
    }
    settings.endGroup();
}

void MainWindow::ouvrirFichierRecent(bool arg1)
{
    if(cheminFichierRecent.isEmpty()){
        return;
    } 

    QDomDocument dom;
    QFile xml_doc(cheminFichierRecent);
    if(xml_doc.exists()){
        if(!xml_doc.open(QIODevice::ReadOnly))
        {
            //QMessageBox::warning(this, "Erreur à l'ouverture du document", "Le document <b>grille.xml</b> n'a pas pu être ouvert.");
            return;
        }

        if(!dom.setContent(&xml_doc))
        {
             xml_doc.close();
             //QMessageBox::warning(this, "Erreur à l'ouverture du document XML", "Le document <b>grille.xml</b> n'a pas pu être attribué à l'objet QDomDocument.");
             return;
        }
    } else {
        QMessageBox::warning(this,tr("File missed"), tr("This translation file doesn't exists anymore"));
        return;
    }
    xml_doc.close();

    QDomElement l10n_element=dom.documentElement();
    ui->bundleName->setText(l10n_element.attribute("name"));

    QDomNodeList langNodesList=l10n_element.childNodes();
    int langNbreNode=langNodesList.count();

    ui->translatedLangList->clear();

    for(int i=0;i<langNbreNode;++i) {
        QDomNode langNode=langNodesList.at(i);
        QDomElement elementLang=langNode.toElement();

        QString langName=elementLang.attribute("name");

        QListWidgetItem *itemLang=new QListWidgetItem(countryCode.key(langName));
        itemLang->setIcon(QIcon(":/new/prefix1/images/point.png"));

        ui->translatedLangList->addItem(itemLang);

        //---Nouveau code---------------------------------------
        QDomNodeList entryNodesList=elementLang.childNodes();
        int entryNbreNode=entryNodesList.count();

        ui->zoneTraduction->clearContents();
        ui->zoneTraduction->setRowCount(0);

        for(int i=0;i<entryNbreNode;++i) {
            QDomNode entryNode=entryNodesList.at(i);
            QDomElement elementEntry=entryNode.toElement();

            QString key=elementEntry.attribute("key");
            QTableWidgetItem *itemKey=new QTableWidgetItem(key);
            QTableWidgetItem *itemValue=new QTableWidgetItem("");

            if(generalKeys.contains(key) || pullToRefreshKeys.contains(key) || calendarKeys.contains(key)){
                itemKey->setForeground(QColor(CODENAMEONE_KEYS_COLOR));
            } else {
                itemKey->setForeground(QColor(OTHERS_KEYS_COLOR));              
            }

            itemKey->setFont(QFont(itemKey->font().family(),10,QFont::Bold));
            //itemKey->setFlags(Qt::NoItemFlags);

            int ligne = ui->zoneTraduction->rowCount();
            ui->zoneTraduction->insertRow(ligne);

            ui->zoneTraduction->setItem(ligne,0,itemKey);
            ui->zoneTraduction->setItem(ligne,1,itemValue);
        }
        //---Fin nouveau code-----------------------------------
    }

    ui->zoneTraduction->horizontalHeaderItem(0)->setText(QString("KEY - %1 items").arg(ui->zoneTraduction->rowCount()));

    QFileInfo fileInfo(cheminFichierRecent);
    QString projectFolderPath=fileInfo.canonicalPath();
    QString srcPathFolder=QFileInfo(projectFolderPath).canonicalPath()+"/src";

    QDir srcFolder(srcPathFolder);
    if(srcFolder.exists()){
        FICHIER_TRANSLATE=cheminFichierRecent;
        ui->cheminDossierSource->setText(QFileInfo(projectFolderPath).canonicalPath());
    }
}

void MainWindow::on_translatedLangList_itemSelectionChanged()
{
    if(ui->translatedLangList->selectedItems().size()==0){
        ui->countryCode->setCurrentIndex(0);
        return;
    }

    int count=ui->zoneTraduction->rowCount();
    if(count==0){
        QMessageBox::warning(this, tr("Contents missed!"), tr("You have to scan your project before."));
        return;
    }

    ui->countryCode->setCurrentText(ui->translatedLangList->currentItem()->text());

    for(int i=0;i<count;++i){
        QTableWidgetItem *itemValue=ui->zoneTraduction->item(i,1);
        itemValue->setText("");
    }

    //---------------------
    QListWidgetItem *selectedItem=ui->translatedLangList->currentItem();
    QString selectedLang=selectedItem->text();
    ui->currentLang->setText(selectedLang.toUpper().mid(0,selectedLang.length()-5));

    import->setText(QString("Import %1...").arg(ui->currentLang->text()));
    exportCSV->setText(QString("Export %1 in CSV format...").arg(ui->currentLang->text()));
    exportProperties->setText(QString("Export %1 in Java Properties format...").arg(ui->currentLang->text()));
    exportJSON->setText(QString("Export %1 in JSON format...").arg(ui->currentLang->text()));

    QDomDocument dom;
    QFile xml_doc(FICHIER_TRANSLATE);
    if(xml_doc.exists()){
        if(!xml_doc.open(QIODevice::ReadOnly))
        {
            //QMessageBox::warning(this, "Erreur à l'ouverture du document", "Le document <b>grille.xml</b> n'a pas pu être ouvert.");
            return;
        }

        if(!dom.setContent(&xml_doc))
        {
             xml_doc.close();
             //QMessageBox::warning(this, "Erreur à l'ouverture du document XML", "Le document <b>grille.xml</b> n'a pas pu être attribué à l'objet QDomDocument.");
             return;
        }
    }
    xml_doc.close();

    QString codePays=countryCode.value(selectedItem->text());

    QDomElement l10n_element=dom.documentElement();
    QDomNodeList langNodesList=l10n_element.childNodes();
    int langNbreNode=langNodesList.count();

    for(int i=0;i<langNbreNode;i++) {
        QDomNode langNode=langNodesList.at(i);
        QDomElement elementLang=langNode.toElement();

        if(elementLang.attribute("name")==codePays){
            QDomNodeList entryNodesList=elementLang.childNodes();
            int entryNbreNode=entryNodesList.count();

            for(int i=0;i<entryNbreNode;i++) {
                QDomNode entryNode=entryNodesList.at(i);
                QDomElement elementEntry=entryNode.toElement();

                QTableWidgetItem *itemValue=ui->zoneTraduction->item(i,1);
                itemValue->setText(elementEntry.attribute("value"));
            }
        }
    }
}

void MainWindow::on_actionAdd_General_Keys_triggered()
{
   ajouterCodenameOneKeys(generalKeys);
}

void MainWindow::on_actionAdd_Calendar_Keys_triggered()
{
    ajouterCodenameOneKeys(calendarKeys);
}

void MainWindow::on_actionAdd_PullToRefresh_Keys_triggered()
{
    ajouterCodenameOneKeys(pullToRefreshKeys);
}

void MainWindow::ajouterCodenameOneKeys(const QStringList &keys)
{
    QString cheminProjet=ui->cheminDossierSource->text();
    if(cheminProjet.isEmpty()){
        QMessageBox::warning(this,tr("Project folder missed"),tr("You have to choose a <b>Codename One project folder</b> before"));
        return;
    }

    if(ui->translatedLangList->selectedItems().size()==0){
        QMessageBox::warning(this, tr("Language missed"), tr("You have to select a language before"));
        return;
    }

    for(int i=0;i<ui->zoneTraduction->rowCount();++i){
        QTableWidgetItem *item=ui->zoneTraduction->item(i,0);
        if(keys.contains(item->text())){
            QMessageBox::warning(this, tr("Already added !!"), tr("These properties are already added !"));
            return;
        }
    }

    for (QString key : keys) {
        QTableWidgetItem *itemKey=new QTableWidgetItem(key);
        QTableWidgetItem *itemValue=new QTableWidgetItem("");

        itemKey->setForeground(QColor(CODENAMEONE_KEYS_COLOR));
        itemKey->setFont(QFont(itemKey->font().family(),10,QFont::Bold));

        int ligne = ui->zoneTraduction->rowCount();
        ui->zoneTraduction->insertRow(ligne);

        ui->zoneTraduction->setItem(ligne,0,itemKey);
        ui->zoneTraduction->setItem(ligne,1,itemValue);

        ui->zoneTraduction->scrollToItem(itemKey);
    }
    ui->zoneTraduction->horizontalHeaderItem(0)->setText(QString("KEY - %1 items").arg(ui->zoneTraduction->rowCount()));
}

void MainWindow::exporterEnCSV()
{
    if(ui->translatedLangList->selectedItems().size()==0){
        QMessageBox::warning(this, tr("Language selection missed"), tr("You have to select a language before"));
        return;
    }

    int nbreElements=ui->zoneTraduction->rowCount();

    if(nbreElements==0){
        QMessageBox::warning(this, tr("No content"), tr("There is no content to export in CSV format"));
        return;
    }

    QString fileName=QFileDialog::getSaveFileName(this,"Enter the CSV file name", ui->currentLang->text(),tr("CSV file (*.csv)"));
    if(fileName.isEmpty()){
        return;
    }

    QFile fichierCSV(fileName);
    fichierCSV.open(QIODevice::WriteOnly);
    QTextStream stream(&fichierCSV);

    for (int i=0;i<nbreElements;++i) {
        QTableWidgetItem *itemKey=ui->zoneTraduction->item(i,0);
        QTableWidgetItem *itemValue=ui->zoneTraduction->item(i,1);
        stream<<itemKey->text()+";"+itemValue->text()+";\n";
    }
    fichierCSV.close();
}

void MainWindow::exporterEnProperties()
{
    if(ui->translatedLangList->selectedItems().size()==0){
        QMessageBox::warning(this, tr("Language selection missed"), tr("You have to select a language before"));
        return;
    }

    int nbreElements=ui->zoneTraduction->rowCount();

    if(nbreElements==0){
        QMessageBox::warning(this, tr("No content"), tr("There is no content to export in Java Properties format"));
        return;
    }

    QString fileName=QFileDialog::getSaveFileName(this,tr("Enter the Java Properties file name"), ui->currentLang->text(),tr("Java Properties file (*.properties)"));
    if(fileName.isEmpty()){
        return;
    }

    QFile fichierProp(fileName);
    fichierProp.open(QIODevice::WriteOnly);
    QTextStream stream(&fichierProp);

    stream<<"#Export from Codename One Linguist\n#"+CN1LINGUIST_GITHUB_PAGE+"\n#"+QDate::currentDate().toString()+"\n";
    for (int i=0;i<nbreElements;++i) {
        QTableWidgetItem *itemKey=ui->zoneTraduction->item(i,0);
        QTableWidgetItem *itemValue=ui->zoneTraduction->item(i,1);
        stream<<itemKey->text().replace(" ","\\ ")+"="+itemValue->text()+"\n";
    }
    fichierProp.close();
}

void MainWindow::exporterEnJSON()
{
    if(ui->translatedLangList->selectedItems().size()==0){
        QMessageBox::warning(this, tr("Language selection missed"), tr("You have to select a language before"));
        return;
    }

    int nbreElements=ui->zoneTraduction->rowCount();

    if(nbreElements==0){
        QMessageBox::warning(this, tr("No content"), tr("There is no content to export in JSON format"));
        return;
    }

    QString fileName=QFileDialog::getSaveFileName(this,"Enter the JSON file name", ui->currentLang->text(),tr("JSON file (*.json)"));
    if(fileName.isEmpty()){
        return;
    }

    QFile fichierJSON(fileName);
    fichierJSON.open(QIODevice::WriteOnly);
    QTextStream stream(&fichierJSON);

    stream<<"{\n";
    for (int i=0;i<nbreElements;++i) {
        QTableWidgetItem *itemKey=ui->zoneTraduction->item(i,0);
        QTableWidgetItem *itemValue=ui->zoneTraduction->item(i,1);

        if(i==nbreElements-1){
            stream<<"\""+itemKey->text()+"\":\""+itemValue->text()+"\"\n";
        } else {
            stream<<"\""+itemKey->text()+"\":\""+itemValue->text()+"\",\n";
        }
    }
    stream<<"}";
    fichierJSON.close();
}

void MainWindow::importerSauvegarde()
{
    if(ui->translatedLangList->selectedItems().size()==0){
        QMessageBox::warning(this, tr("Language selection missed"), tr("You have to select a language before"));
        return;
    }

    QString fileName=QFileDialog::getOpenFileName(this,tr("Choose the file"), ".",tr("CSV or Properties or JSON file (*.csv *.properties *json)"));
    if(fileName.isEmpty()){
        return;
    }

    QFile fichier(fileName);
    fichier.open(QIODevice::ReadOnly);
    QTextStream stream(&fichier);

    ui->zoneTraduction->clearContents();
    ui->zoneTraduction->setRowCount(0);

    while (!stream.atEnd()) {
        QString contenu=stream.readLine();

        if(fileName.endsWith(".csv")){
            QTableWidgetItem *itemKey=new QTableWidgetItem(contenu.split(";").at(0));
            QTableWidgetItem *itemValue=new QTableWidgetItem(contenu.split(";").at(1));

            QString key=itemKey->text();

            if(generalKeys.contains(key) || pullToRefreshKeys.contains(key) || calendarKeys.contains(key)){
                itemKey->setForeground(QColor(CODENAMEONE_KEYS_COLOR));
            } else {
                itemKey->setForeground(QColor(OTHERS_KEYS_COLOR));
            }

            itemKey->setFont(QFont(itemKey->font().family(),10,QFont::Bold));

            int ligne = ui->zoneTraduction->rowCount();
            ui->zoneTraduction->insertRow(ligne);

            ui->zoneTraduction->setItem(ligne,0,itemKey);
            ui->zoneTraduction->setItem(ligne,1,itemValue);

        } else if(fileName.endsWith(".properties")) {

            if(!contenu.startsWith("#")){
                QString key=contenu.split("=").at(0);
                key.replace("\\ "," ");

                QTableWidgetItem *itemKey=new QTableWidgetItem(key);
                QTableWidgetItem *itemValue=new QTableWidgetItem(contenu.split("=").at(1));

                if(generalKeys.contains(key) || pullToRefreshKeys.contains(key) || calendarKeys.contains(key)){
                    itemKey->setForeground(QColor(CODENAMEONE_KEYS_COLOR));
                } else {
                    itemKey->setForeground(QColor(OTHERS_KEYS_COLOR));
                }

                itemKey->setFont(QFont(itemKey->font().family(),10,QFont::Bold));

                int ligne = ui->zoneTraduction->rowCount();
                ui->zoneTraduction->insertRow(ligne);

                ui->zoneTraduction->setItem(ligne,0,itemKey);
                ui->zoneTraduction->setItem(ligne,1,itemValue);
            }

        } else if(fileName.endsWith(".json")) {
            QStringList data=contenu.split(":");

            if(data.size()==2){
                QString key=data.at(0);
                QString value=data.at(1);

                key.remove("\"");
                value.remove("\"").remove(",");

                QTableWidgetItem *itemKey=new QTableWidgetItem(key);
                QTableWidgetItem *itemValue=new QTableWidgetItem(value);

                if(generalKeys.contains(key) || pullToRefreshKeys.contains(key) || calendarKeys.contains(key)){
                    itemKey->setForeground(QColor(CODENAMEONE_KEYS_COLOR));
                } else {
                    itemKey->setForeground(QColor(OTHERS_KEYS_COLOR));
                }

                itemKey->setFont(QFont(itemKey->font().family(),10,QFont::Bold));

                int ligne = ui->zoneTraduction->rowCount();
                ui->zoneTraduction->insertRow(ligne);

                ui->zoneTraduction->setItem(ligne,0,itemKey);
                ui->zoneTraduction->setItem(ligne,1,itemValue);
            }

        }

    }

    ui->zoneTraduction->horizontalHeaderItem(0)->setText(QString("KEY - %1 items").arg(ui->zoneTraduction->rowCount()));
    fichier.close();
}

void MainWindow::supprimerPropriete()
{
    if(ui->zoneTraduction->rowCount()==0){
        QMessageBox::warning(this, tr("No property"), tr("There is no property to remove"));
        return;
    }

    if(ui->translatedLangList->selectedItems().size()==0){
        QMessageBox::warning(this, tr("Language missed"), tr("You have to select a language before"));
        return;
    }

    QDomDocument dom;
    QFile xml_doc(FICHIER_TRANSLATE);
    if(xml_doc.exists()){
        if(!xml_doc.open(QIODevice::ReadOnly))
        {
            //QMessageBox::warning(this, "Erreur à l'ouverture du document", "Le document <b>grille.xml</b> n'a pas pu être ouvert.");
            return;
        }

        if(!dom.setContent(&xml_doc))
        {
             xml_doc.close();
             //QMessageBox::warning(this, "Erreur à l'ouverture du document XML", "Le document <b>grille.xml</b> n'a pas pu être attribué à l'objet QDomDocument.");
             return;
        }
    }
    xml_doc.close();

    int ligneCourante=ui->zoneTraduction->currentRow();
    QString key=ui->zoneTraduction->item(ligneCourante,0)->text();
    ui->zoneTraduction->removeRow(ui->zoneTraduction->currentRow());

    QDomElement l10n_element=dom.documentElement();
    QDomNodeList langNodesList=l10n_element.childNodes();
    int langNbreNode=langNodesList.count();

    for(int i=0;i<langNbreNode;i++) {
        QDomNode langNode=langNodesList.at(i);
        QDomElement elementLang=langNode.toElement();

        //if(elementLang.attribute("name")==codePays){
            QDomNodeList entryNodesList=elementLang.childNodes();
            int entryNbreNode=entryNodesList.count();

            for(int j=0;j<entryNbreNode;j++) {
                QDomNode entryNode=entryNodesList.at(j);
                QDomElement elementEntry=entryNode.toElement();

                if(elementEntry.attribute("key")==key){
                    elementLang.removeChild(elementEntry);

                    QString xml = dom.toString(3);
                    xml_doc.setFileName(FICHIER_TRANSLATE);
                    xml_doc.open(QIODevice::WriteOnly);
                    QTextStream stream(&xml_doc);
                    stream.setCodec("UTF-8");
                    stream<<xml;
                    xml_doc.close();
                    //return;
                }
            }

        //} //fin première if

    } //fin première boucle for
}

void MainWindow::editerCleValeur()
{
    if(ui->zoneTraduction->rowCount()==0){
        QMessageBox::warning(this, tr("No property"), tr("There is no property to edit"));
        return;
    }

    editProperty->addPropertyToEdit(ui->zoneTraduction->currentItem()->text());
    editProperty->show();
    editProperty->raise();
    editProperty->activateWindow();
}

void MainWindow::dupliquerCleVersValeur()
{
    if(ui->zoneTraduction->rowCount()==0){
        QMessageBox::warning(this, tr("No property"), tr("There is no key to duplicate"));
        return;
    }

    int ligneCourante=ui->zoneTraduction->currentRow();
    QTableWidgetItem *itemKey=ui->zoneTraduction->item(ligneCourante,0);
    ui->zoneTraduction->item(ligneCourante,1)->setText(itemKey->text());
}

void MainWindow::on_addKey_clicked()
{
    if(ui->key->text().isEmpty()){
        return;
    }

    QString cheminProjet=ui->cheminDossierSource->text();
    if(cheminProjet.isEmpty()){
        QMessageBox::warning(this,tr("Project folder missed"),tr("You have to choose a <b>Codename One project folder</b> before"));
        return;
    }

    if(ui->translatedLangList->selectedItems().size()==0){
        QMessageBox::warning(this, tr("Language missed"), tr("You have to select a language before"));
        return;
    }   

    int nbreLignes=ui->zoneTraduction->rowCount();
    for(int i=0;i<nbreLignes;i++){
        QTableWidgetItem *item=ui->zoneTraduction->item(i,0);
        if(ui->key->text()==item->text()){
            QMessageBox::warning(this, tr("Already added !!"), tr("This property is already added !"));
            return;
        }
    }

    QTableWidgetItem *itemKey=new QTableWidgetItem(ui->key->text());
    QTableWidgetItem *itemValue=new QTableWidgetItem("");

    //itemKey->setFlags(Qt::NoItemFlags);
    itemKey->setForeground(QColor(OTHERS_KEYS_COLOR));
    itemKey->setFont(QFont(itemKey->font().family(),10,QFont::Bold));

    int ligne = ui->zoneTraduction->rowCount();
    ui->zoneTraduction->insertRow(ligne);

    ui->zoneTraduction->setItem(ligne,0,itemKey);
    ui->zoneTraduction->setItem(ligne,1,itemValue);

    ui->zoneTraduction->horizontalHeaderItem(0)->setText(QString("KEY - %1 items").arg(ui->zoneTraduction->rowCount()));

    ui->key->clear();
    ui->zoneTraduction->scrollToItem(itemKey);
}

void MainWindow::on_key_returnPressed()
{
    on_addKey_clicked();
}

void MainWindow::on_zoneTraduction_doubleClicked(const QModelIndex &index)
{
    if(ui->translatedLangList->selectedItems().count()==0){
        QMessageBox::warning(this, tr("Language missed"), tr("You have to select a language before"));
    }
}

void MainWindow::on_actionAdd_RTL_Key_triggered()
{
    QString cheminProjet=ui->cheminDossierSource->text();
    if(cheminProjet.isEmpty()){
        QMessageBox::warning(this,tr("Project folder missed"),tr("You have to choose a <b>Codename One project folder</b> before"));
        return;
    }

    if(ui->translatedLangList->selectedItems().size()==0){
        QMessageBox::warning(this, tr("Language missed"), tr("You have to select a language before"));
        return;
    }

    bool ok;
    QStringList rtlKeyValues={"true", "false"};
    QString value=QInputDialog::getItem(this,tr("RTL Value"), tr("Choose a value (true/false)"), rtlKeyValues, 0, false, &ok);
    if(!ok){
        return;
    }

    for(int i=0;i<ui->zoneTraduction->rowCount();++i){
        QTableWidgetItem *item=ui->zoneTraduction->item(i,0);
        if(item->text()=="@rtl"){
            QMessageBox::warning(this, tr("Already added !!"), tr("This property is already added !"));
            return;
        }
    }

    if(!value.isEmpty()){
        if(value!="true" && value!="false"){
            QMessageBox::warning(this, tr("Wrong value"), tr("The value must be true or false"));
            return;
        }

        QTableWidgetItem *itemKey=new QTableWidgetItem("@rtl");
        QTableWidgetItem *itemValue=new QTableWidgetItem(value);

        //itemKey->setFlags(Qt::NoItemFlags);
        itemKey->setForeground(QColor(CODENAMEONE_KEYS_COLOR));
        itemKey->setToolTip("Refers to languages that are written from right to left (RTL) like Arabic and Hebrew");
        itemKey->setFont(QFont(itemKey->font().family(),10,QFont::Bold));

        int ligne = ui->zoneTraduction->rowCount();
        ui->zoneTraduction->insertRow(ligne);

        ui->zoneTraduction->setItem(ligne,0,itemKey);
        ui->zoneTraduction->setItem(ligne,1,itemValue);

        ui->zoneTraduction->horizontalHeaderItem(0)->setText(QString("KEY - %1 items").arg(ui->zoneTraduction->rowCount()));

        ui->zoneTraduction->scrollToItem(itemKey);
    }
}

void MainWindow::on_zoneTraduction_itemChanged(QTableWidgetItem *item)
{
    if(item->column()==1 && item->text()!=""){
        int nbreValue=0;
        for (int i=0;i<ui->zoneTraduction->rowCount();++i) {
            if(!ui->zoneTraduction->item(i,1)->text().isEmpty()){
                nbreValue++;
            }
        }

        ui->zoneTraduction->horizontalHeaderItem(1)->setText(QString("VALUE - %1 items").arg(nbreValue));

        if(ui->translatedLangList->selectedItems().size()==0){
            return;
        }

        QString cheminProjet=ui->cheminDossierSource->text();
        if(cheminProjet.isEmpty()){
            return;
        }

        QString l10nPathFolder=cheminProjet+QDir::separator()+"l10n";
        QDir l10nFolder(l10nPathFolder);
        if(!l10nFolder.exists()){
            l10nFolder.mkdir(l10nPathFolder);
            QString projectFolderName=QFileInfo(cheminProjet).baseName();
            FICHIER_TRANSLATE=l10nPathFolder+QDir::separator()+projectFolderName+".cn1l";
        }

        if(QFile::exists(FICHIER_TRANSLATE)){
            ajouterDonnees(true);
            statusBar()->showMessage("Auto saved", 1000);
        }
    }
}

void MainWindow::on_actionEnable_Disable_Fusion_Theme_triggered()
{
    QSettings settings("cn1ling-config.ini", QSettings::IniFormat);
    settings.beginGroup("General");
    settings.setValue("ThemeFusion", ui->actionEnable_Disable_Fusion_Theme->isChecked());
    settings.endGroup();

    int reponse=QMessageBox::information(this,tr("Restart needed"),tr("You have to restart the application to apply the new theme. Do you want to do it now ?"), QMessageBox::Yes|QMessageBox::No);
    if(reponse==QMessageBox::Yes){
        qApp->quit();
        QProcess::startDetached(qApp->arguments()[0],QStringList());
    }
}

void MainWindow::on_viderZoneTraduction_clicked()
{
    if(ui->zoneTraduction->rowCount()==0){
        QMessageBox::warning(this,tr("Delete"),tr("There is no keys and values to remove."));
        return;
    }

    int reponse=QMessageBox::question(this,tr("Confirm"),tr("Do you really want to remove all keys and values ? It will not be saved automatically"), QMessageBox::Yes|QMessageBox::No);
    if(reponse==QMessageBox::Yes){
        ui->zoneTraduction->clearContents();
        ui->zoneTraduction->setRowCount(0);

        ui->zoneTraduction->horizontalHeaderItem(0)->setText("KEY");
        ui->zoneTraduction->horizontalHeaderItem(1)->setText("VALUE");
    }
}
