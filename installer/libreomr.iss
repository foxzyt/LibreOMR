; =====================================================================
; LibreOMR Desktop - Inno Setup Script
; Optical Mark Recognition Desktop Application
; Copyright (C) 2026 Bernardo Alvim
; =====================================================================

#define MyAppName "LibreOMR Desktop"
#define MyAppVersion "1.2.0"
#define MyAppPublisher "Bernardo Alvim"
#define MyAppURL "https://github.com/foxzyt/LibreOMR"
#define MyAppExeName "libreomr-desktop.exe"
#define MyAppCliName "libreomr.exe"

[Setup]
; App Identity
AppId={{7E1C8302-3B44-4F90-8E15-927E26B64FA8}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}/releases

; Installation Directory
DefaultDirName={autopf}\LibreOMR
DefaultGroupName=LibreOMR
AllowNoIcons=yes

; Visual & UI Customization
WizardStyle=modern
SetupIconFile=..\app\resources\app.ico
UninstallDisplayIcon={app}\{#MyAppExeName}
LicenseFile=..\LICENSE

; Compression & Optimization
Compression=lzma2/ultra64
SolidCompression=yes

; Permissions & Target Architecture
ArchitecturesInstallIn64BitMode=x64compatible
PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog

; Output Settings
OutputDir=..\dist
OutputBaseFilename=LibreOMR-Setup-{#MyAppVersion}-x64

[Languages]
Name: "brazilianportuguese"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"
Name: "german"; MessagesFile: "compiler:Languages\German.isl"
Name: "italian"; MessagesFile: "compiler:Languages\Italian.isl"

[CustomMessages]
; --- Brazilian Portuguese ---
brazilianportuguese.TypeFull=Instalação Completa (Aplicativo + Todos os Gabaritos + CLI)
brazilianportuguese.TypeCompact=Instalação Mínima (Apenas o Aplicativo Desktop)
brazilianportuguese.TypeCustom=Instalação Personalizada

brazilianportuguese.CompApp=LibreOMR Desktop (Aplicativo Principal)
brazilianportuguese.CompCli=Ferramenta de Linha de Comando (libreomr-cli)
brazilianportuguese.CompTemplates=Pacotes de Gabaritos para Impressão (PNG e JPG)
brazilianportuguese.CompLangPt=Gabaritos em Português
brazilianportuguese.CompLangEn=Gabaritos em Inglês (Answer Sheets)
brazilianportuguese.CompLangEs=Gabaritos em Espanhol (Hojas de Respuestas)
brazilianportuguese.CompLangFr=Gabaritos em Francês (Feuilles de Réponses)
brazilianportuguese.CompLangDe=Gabaritos em Alemão (Antwortbogen)
brazilianportuguese.CompLangIt=Gabaritos em Italiano (Fogli Risposte)

brazilianportuguese.CompTemplatesStd=Modelos A4 Tradicionais (10 a 100 Questões, 3 a 5 Alternativas)
brazilianportuguese.CompTemplates2in1=Modelos Econômicos 2-em-1 (Meia Folha A4, 10 a 30 Questões)
brazilianportuguese.CompTemplates4in1=Modelos Compactos 4-em-1 (Quarto de Folha A4, 10 a 20 Questões)
brazilianportuguese.CompTemplatesCE=Gabaritos de Concurso Público (Certo/Errado - Folha Cheia, Meia e Quarto)

brazilianportuguese.GroupOther=Tarefas Adicionais:
brazilianportuguese.TaskDesktopIcon=Criar atalho na Área de Trabalho
brazilianportuguese.TaskContextMenu=Integrar ao Menu de Contexto do Windows (Clique com botão direito em imagens e pastas)
brazilianportuguese.TaskAddToPath=Adicionar ferramentas do LibreOMR à variável de ambiente PATH

brazilianportuguese.ContextMenuFile=Avaliar com LibreOMR
brazilianportuguese.ContextMenuFolder=Avaliar Pasta com LibreOMR

brazilianportuguese.ShortcutTemplates=Modelos de Gabaritos para Impressão

brazilianportuguese.WebView2Title=Dependência Microsoft Edge WebView2
brazilianportuguese.WebView2Desc=O LibreOMR Desktop requer o componente Microsoft Edge WebView2 Evergreen Runtime.
brazilianportuguese.WebView2Downloading=Baixando Microsoft Edge WebView2 Evergreen Bootstrapper...
brazilianportuguese.WebView2Installing=Instalando Microsoft Edge WebView2 Runtime... Aguarde...
brazilianportuguese.WebView2Error=Não foi possível baixar ou instalar o Microsoft Edge WebView2 automaticamente. Se o aplicativo não iniciar, instale o WebView2 Runtime manualmente a partir do site da Microsoft.

brazilianportuguese.AlreadyInstalledTitle=LibreOMR Desktop já está instalado
brazilianportuguese.AlreadyInstalledDesc=Uma versão do LibreOMR Desktop já foi detectada neste computador. O que você deseja fazer?
brazilianportuguese.AlreadyInstalledModify=&Modificar / Atualizar
brazilianportuguese.AlreadyInstalledModifySub=Atualizar os arquivos, reinstalar componentes e sincronizar os pacotes de gabaritos.
brazilianportuguese.AlreadyInstalledUninstall=&Desinstalar
brazilianportuguese.AlreadyInstalledUninstallSub=Remover completamente o LibreOMR Desktop e seus componentes deste computador.

; --- English ---
english.TypeFull=Full Installation (Application + All Answer Sheets + CLI)
english.TypeCompact=Minimal Installation (Desktop Application Only)
english.TypeCustom=Custom Installation

english.CompApp=LibreOMR Desktop (Main Application)
english.CompCli=Command Line Interface (libreomr-cli)
english.CompTemplates=Printable Answer Sheet Packages (PNG and JPG)
english.CompLangPt=Answer Sheets in Portuguese
english.CompLangEn=Answer Sheets in English
english.CompLangEs=Answer Sheets in Spanish
english.CompLangFr=Answer Sheets in French
english.CompLangDe=Answer Sheets in German
english.CompLangIt=Answer Sheets in Italian

english.CompTemplatesStd=Standard A4 Answer Sheets (10 to 100 Questions, 3 to 5 Options)
english.CompTemplates2in1=Economical 2-in-1 Sheets (Half A4 Sheet, 10 to 30 Questions)
english.CompTemplates4in1=Compact 4-in-1 Sheets (Quarter A4 Sheet, 10 to 20 Questions)
english.CompTemplatesCE=Assessment Exam Sheets (True/False - Full, Half and Quarter Sheet)

english.GroupOther=Additional Tasks:
english.TaskDesktopIcon=Create a desktop shortcut
english.TaskContextMenu=Integrate with Windows Context Menu (Right-click on images and folders)
english.TaskAddToPath=Add LibreOMR tools to system PATH environment variable

english.ContextMenuFile=Grade with LibreOMR
english.ContextMenuFolder=Grade Folder with LibreOMR

english.ShortcutTemplates=Printable Answer Sheet Templates

english.WebView2Title=Microsoft Edge WebView2 Dependency
english.WebView2Desc=LibreOMR Desktop requires the Microsoft Edge WebView2 Evergreen Runtime.
english.WebView2Downloading=Downloading Microsoft Edge WebView2 Evergreen Bootstrapper...
english.WebView2Installing=Installing Microsoft Edge WebView2 Runtime... Please wait...
english.WebView2Error=Could not download or install Microsoft Edge WebView2 automatically. If the application does not start, please install the WebView2 Runtime manually from Microsoft's website.

english.AlreadyInstalledTitle=LibreOMR Desktop is already installed
english.AlreadyInstalledDesc=An existing installation of LibreOMR Desktop was detected on this computer. What would you like to do?
english.AlreadyInstalledModify=&Modify / Update
english.AlreadyInstalledModifySub=Update existing files, reinstall components, and refresh answer sheet packages.
english.AlreadyInstalledUninstall=&Uninstall
english.AlreadyInstalledUninstallSub=Completely remove LibreOMR Desktop and all its components from this computer.

; --- Spanish ---
spanish.TypeFull=Instalación Completa (Aplicación + Todas las Hojas de Respuestas + CLI)
spanish.TypeCompact=Instalación Mínima (Solo la Aplicación Desktop)
spanish.TypeCustom=Instalación Personalizada

spanish.CompApp=LibreOMR Desktop (Aplicación Principal)
spanish.CompCli=Herramienta de Línea de Comandos (libreomr-cli)
spanish.CompTemplates=Paquetes de Hojas de Respuestas para Impresión (PNG y JPG)
spanish.CompLangPt=Hojas de Respuestas en Portugués
spanish.CompLangEn=Hojas de Respuestas en Inglés
spanish.CompLangEs=Hojas de Respuestas en Español
spanish.CompLangFr=Hojas de Respuestas en Francés
spanish.CompLangDe=Hojas de Respuestas en Alemán
spanish.CompLangIt=Hojas de Respuestas en Italiano

spanish.CompTemplatesStd=Modelos A4 Estándar (10 a 100 Preguntas, 3 a 5 Opciones)
spanish.CompTemplates2in1=Modelos Económicos 2-en-1 (Media Hoja A4, 10 a 30 Preguntas)
spanish.CompTemplates4in1=Modelos Compactos 4-en-1 (Cuarto de Hoja A4, 10 a 20 Preguntas)
spanish.CompTemplatesCE=Hojas de Examen Público (Verdadero/Falso - Hoja Completa, Media y Cuarto)

spanish.GroupOther=Tareas Adicionales:
spanish.TaskDesktopIcon=Crear un acceso directo en el Escritorio
spanish.TaskContextMenu=Integrar al Menú Contextual de Windows (Clic derecho en imágenes y carpetas)
spanish.TaskAddToPath=Añadir las herramientas de LibreOMR a la variable de entorno PATH

spanish.ContextMenuFile=Calificar con LibreOMR
spanish.ContextMenuFolder=Calificar Carpeta con LibreOMR

spanish.ShortcutTemplates=Modelos de Hojas de Respuestas para Impresión

spanish.WebView2Title=Dependencia Microsoft Edge WebView2
spanish.WebView2Desc=LibreOMR Desktop requiere el componente Microsoft Edge WebView2 Evergreen Runtime.
spanish.WebView2Downloading=Descargando Microsoft Edge WebView2 Evergreen Bootstrapper...
spanish.WebView2Installing=Instalando Microsoft Edge WebView2 Runtime... Espere por favor...
spanish.WebView2Error=No se pudo descargar o instalar Microsoft Edge WebView2 automáticamente. Si la aplicación no se inicia, instale el WebView2 Runtime manualmente desde el sitio web de Microsoft.

spanish.AlreadyInstalledTitle=LibreOMR Desktop ya está instalado
spanish.AlreadyInstalledDesc=Se ha detectado una versión existente de LibreOMR Desktop en este equipo. ¿Qué desea hacer?
spanish.AlreadyInstalledModify=&Modificar / Actualizar
spanish.AlreadyInstalledModifySub=Actualizar archivos existentes, componentes y paquetes de hojas de respuestas.
spanish.AlreadyInstalledUninstall=&Desinstalar
spanish.AlreadyInstalledUninstallSub=Desinstalar completamente LibreOMR Desktop de este equipo.

; --- French ---
french.TypeFull=Installation Complète (Application + Toutes les Feuilles de Réponses + CLI)
french.TypeCompact=Installation Minimale (Application Bureau Seule)
french.TypeCustom=Installation Personnalisée

french.CompApp=LibreOMR Desktop (Application Principale)
french.CompCli=Interface en Ligne de Commande (libreomr-cli)
french.CompTemplates=Feuilles de Réponses pour Impression (PNG et JPG)
french.CompLangPt=Feuilles de Réponses en Portugais
french.CompLangEn=Feuilles de Réponses en Anglais
french.CompLangEs=Feuilles de Réponses en Espagnol
french.CompLangFr=Feuilles de Réponses en Français
french.CompLangDe=Feuilles de Réponses en Allemand
french.CompLangIt=Feuilles de Réponses en Italien

french.CompTemplatesStd=Modèles A4 Standard (10 à 100 Questions, 3 à 5 Choix)
french.CompTemplates2in1=Modèles Économiques 2-en-1 (Demi-Feuille A4, 10 à 30 Questions)
french.CompTemplates4in1=Modèles Compacts 4-en-1 (Quart de Feuille A4, 10 à 20 Questions)
french.CompTemplatesCE=Évaluations de Concours (Vrai/Faux - Feuille Entière, Demi et Quart)

french.GroupOther=Tâches Supplémentaires:
french.TaskDesktopIcon=Créer un raccourci sur le Bureau
french.TaskContextMenu=Intégrer au Menu Contextuel de Windows (Clic droit sur images et dossiers)
french.TaskAddToPath=Ajouter les outils LibreOMR à la variable d'environnement PATH

french.ContextMenuFile=Corriger avec LibreOMR
french.ContextMenuFolder=Corriger le Dossier avec LibreOMR

french.ShortcutTemplates=Feuilles de Réponses Imprimables

french.WebView2Title=Dépendance Microsoft Edge WebView2
french.WebView2Desc=LibreOMR Desktop requiert le composant Microsoft Edge WebView2 Evergreen Runtime.
french.WebView2Downloading=Téléchargement de Microsoft Edge WebView2 Evergreen Bootstrapper...
french.WebView2Installing=Installation de Microsoft Edge WebView2 Runtime... Veuillez patienter...
french.WebView2Error=Impossible de télécharger ou d'installer Microsoft Edge WebView2 automatiquement. Si l'application ne démarre pas, veuillez installer manuellement WebView2 Runtime depuis le site officiel de Microsoft.

french.AlreadyInstalledTitle=LibreOMR Desktop est déjà installé
french.AlreadyInstalledDesc=Une installation existante de LibreOMR Desktop a été détectée sur cet ordinateur. Que souhaitez-vous faire ?
french.AlreadyInstalledModify=&Modifier / Mettre à jour
french.AlreadyInstalledModifySub=Mettre à jour les fichiers existants, réinstaller les composants et actualiser les feuilles de réponses.
french.AlreadyInstalledUninstall=&Désinstaller
french.AlreadyInstalledUninstallSub=Supprimer complètement LibreOMR Desktop et ses composants de cet ordinateur.

; --- German ---
german.TypeFull=Vollständige Installation (Anwendung + Alle Antwortbögen + CLI)
german.TypeCompact=Minimale Installation (Nur Desktop-Anwendung)
german.TypeCustom=Benutzerdefinierte Installation

german.CompApp=LibreOMR Desktop (Hauptanwendung)
german.CompCli=Kommandozeilenwerkzeug (libreomr-cli)
german.CompTemplates=Druckbare Antwortbögen-Pakete (PNG und JPG)
german.CompLangPt=Antwortbögen auf Portugiesisch
german.CompLangEn=Antwortbögen auf Englisch
german.CompLangEs=Antwortbögen auf Spanisch
german.CompLangFr=Antwortbögen auf Französisch
german.CompLangDe=Antwortbögen auf Deutsch
german.CompLangIt=Antwortbögen auf Italienisch

german.CompTemplatesStd=A4 Standard-Antwortbögen (10 bis 100 Fragen, 3 bis 5 Optionen)
german.CompTemplates2in1=Sparsame 2-in-1 Bögen (Halbes A4-Blatt, 10 bis 30 Fragen)
german.CompTemplates4in1=Kompakte 4-in-1 Bögen (Viertel A4-Blatt, 10 bis 20 Fragen)
german.CompTemplatesCE=Prüfungsbögen (Richtig/Falsch - Ganzes, Halbes und Viertel Blatt)

german.GroupOther=Zusätzliche Aufgaben:
german.TaskDesktopIcon=Desktop-Verknüpfung erstellen
german.TaskContextMenu=In das Windows-Kontextmenü integrieren (Rechtsklick auf Bilder und Ordner)
german.TaskAddToPath=LibreOMR-Werkzeuge zur System-PATH-Umgebungsvariable hinzufügen

german.ContextMenuFile=Mit LibreOMR auswerten
german.ContextMenuFolder=Ordner mit LibreOMR auswerten

german.ShortcutTemplates=Druckbare Antwortbögen-Vorlagen

german.WebView2Title=Microsoft Edge WebView2 Abhängigkeit
german.WebView2Desc=LibreOMR Desktop benötigt die Microsoft Edge WebView2 Evergreen Runtime.
german.WebView2Downloading=Microsoft Edge WebView2 Evergreen Bootstrapper wird heruntergeladen...
german.WebView2Installing=Microsoft Edge WebView2 Runtime wird installiert... Bitte warten...
german.WebView2Error=Microsoft Edge WebView2 konnte nicht automatisch installiert werden. Falls die Anwendung nicht startet, installieren Sie die WebView2 Runtime bitte manuell von der Microsoft-Webseite.

german.AlreadyInstalledTitle=LibreOMR Desktop ist bereits installiert
german.AlreadyInstalledDesc=Auf diesem Computer wurde eine bestehende Installation von LibreOMR Desktop gefunden. Was möchten Sie tun?
german.AlreadyInstalledModify=&Ändern / Aktualisieren
german.AlreadyInstalledModifySub=Vorhandene Dateien aktualisieren, Komponenten anpassen und Antwortbögen erneuern.
german.AlreadyInstalledUninstall=&Deinstallieren
german.AlreadyInstalledUninstallSub=LibreOMR Desktop und alle zugehörigen Komponenten vollständig entfernen.

; --- Italian ---
italian.TypeFull=Installazione Completa (Applicazione + Tutti i Fogli Risposte + CLI)
italian.TypeCompact=Installazione Minima (Solo Applicazione Desktop)
italian.TypeCustom=Installazione Personalizzata

italian.CompApp=LibreOMR Desktop (Applicazione Principale)
italian.CompCli=Strumento da Riga di Comando (libreomr-cli)
italian.CompTemplates=Pacchetti Fogli Risposte per la Stampa (PNG e JPG)
italian.CompLangPt=Fogli Risposte in Portoghese
italian.CompLangEn=Fogli Risposte in Inglese
italian.CompLangEs=Fogli Risposte in Spagnolo
italian.CompLangFr=Fogli Risposte in Francese
italian.CompLangDe=Fogli Risposte in Tedesco
italian.CompLangIt=Fogli Risposte in Italiano

italian.CompTemplatesStd=Modelli A4 Standard (da 10 a 100 Domande, da 3 a 5 Opzioni)
italian.CompTemplates2in1=Modelli Economici 2-in-1 (Mezzo Foglio A4, da 10 a 30 Domande)
italian.CompTemplates4in1=Modelli Compatti 4-in-1 (Quarto di Foglio A4, da 10 a 20 Domande)
italian.CompTemplatesCE=Prove di Concorso (Vero/Falso - Foglio Intero, Mezzo e Quarto)

italian.GroupOther=Attività Aggiuntive:
italian.TaskDesktopIcon=Crea un collegamento sul Desktop
italian.TaskContextMenu=Integra nel Menu Contestuale di Windows (Clic destro su immagini e cartelle)
italian.TaskAddToPath=Aggiungi gli strumenti LibreOMR alla variabile d'ambiente PATH

italian.ContextMenuFile=Valuta con LibreOMR
italian.ContextMenuFolder=Valuta Cartella con LibreOMR

italian.ShortcutTemplates=Modelli Fogli Risposte per la Stampa

italian.WebView2Title=Dipendenza Microsoft Edge WebView2
italian.WebView2Desc=LibreOMR Desktop richiede il componente Microsoft Edge WebView2 Evergreen Runtime.
italian.WebView2Downloading=Download di Microsoft Edge WebView2 Evergreen Bootstrapper in corso...
italian.WebView2Installing=Installazione di Microsoft Edge WebView2 Runtime... Attendere...
italian.WebView2Error=Impossibile scaricare o installare Microsoft Edge WebView2 automaticamente. Se l'applicazione non si avvia, installare manualmente WebView2 Runtime dal sito ufficiale Microsoft.

italian.AlreadyInstalledTitle=LibreOMR Desktop è già installato
italian.AlreadyInstalledDesc=È stata rilevata un'installazione esistente di LibreOMR Desktop su questo computer. Cosa desideri fare?
italian.AlreadyInstalledModify=&Modifica / Aggiorna
italian.AlreadyInstalledModifySub=Aggiorna i file esistenti, reinstalla i componenti e rinnova i pacchetti di fogli risposte.
italian.AlreadyInstalledUninstall=&Disinstalla
italian.AlreadyInstalledUninstallSub=Rimuovi completamente LibreOMR Desktop e tutti i suoi componenti da questo computer.

[Types]
Name: "full"; Description: "{cm:TypeFull}"
Name: "compact"; Description: "{cm:TypeCompact}"
Name: "custom"; Description: "{cm:TypeCustom}"; Flags: iscustom

[Components]
Name: "app"; Description: "{cm:CompApp}"; Types: full compact custom; Flags: fixed
Name: "cli"; Description: "{cm:CompCli}"; Types: full custom
Name: "templates"; Description: "{cm:CompTemplates}"; Types: full custom

; Portuguese Templates
Name: "templates\pt"; Description: "{cm:CompLangPt}"; Types: full custom
Name: "templates\pt\std"; Description: "{cm:CompTemplatesStd}"; Types: full custom
Name: "templates\pt\2in1"; Description: "{cm:CompTemplates2in1}"; Types: full custom
Name: "templates\pt\4in1"; Description: "{cm:CompTemplates4in1}"; Types: full custom
Name: "templates\pt\ce"; Description: "{cm:CompTemplatesCE}"; Types: full custom

; English Templates
Name: "templates\en"; Description: "{cm:CompLangEn}"; Types: full custom
Name: "templates\en\std"; Description: "{cm:CompTemplatesStd}"; Types: full custom
Name: "templates\en\2in1"; Description: "{cm:CompTemplates2in1}"; Types: full custom
Name: "templates\en\4in1"; Description: "{cm:CompTemplates4in1}"; Types: full custom
Name: "templates\en\ce"; Description: "{cm:CompTemplatesCE}"; Types: full custom

; Spanish Templates
Name: "templates\es"; Description: "{cm:CompLangEs}"; Types: full custom
Name: "templates\es\std"; Description: "{cm:CompTemplatesStd}"; Types: full custom
Name: "templates\es\2in1"; Description: "{cm:CompTemplates2in1}"; Types: full custom
Name: "templates\es\4in1"; Description: "{cm:CompTemplates4in1}"; Types: full custom
Name: "templates\es\ce"; Description: "{cm:CompTemplatesCE}"; Types: full custom

; French Templates
Name: "templates\fr"; Description: "{cm:CompLangFr}"; Types: full custom
Name: "templates\fr\std"; Description: "{cm:CompTemplatesStd}"; Types: full custom
Name: "templates\fr\2in1"; Description: "{cm:CompTemplates2in1}"; Types: full custom
Name: "templates\fr\4in1"; Description: "{cm:CompTemplates4in1}"; Types: full custom
Name: "templates\fr\ce"; Description: "{cm:CompTemplatesCE}"; Types: full custom

; German Templates
Name: "templates\de"; Description: "{cm:CompLangDe}"; Types: full custom
Name: "templates\de\std"; Description: "{cm:CompTemplatesStd}"; Types: full custom
Name: "templates\de\2in1"; Description: "{cm:CompTemplates2in1}"; Types: full custom
Name: "templates\de\4in1"; Description: "{cm:CompTemplates4in1}"; Types: full custom
Name: "templates\de\ce"; Description: "{cm:CompTemplatesCE}"; Types: full custom

; Italian Templates
Name: "templates\it"; Description: "{cm:CompLangIt}"; Types: full custom
Name: "templates\it\std"; Description: "{cm:CompTemplatesStd}"; Types: full custom
Name: "templates\it\2in1"; Description: "{cm:CompTemplates2in1}"; Types: full custom
Name: "templates\it\4in1"; Description: "{cm:CompTemplates4in1}"; Types: full custom
Name: "templates\it\ce"; Description: "{cm:CompTemplatesCE}"; Types: full custom

[Tasks]
Name: "desktopicon"; Description: "{cm:TaskDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "contextmenu"; Description: "{cm:TaskContextMenu}"; GroupDescription: "{cm:GroupOther}"; Flags: checkablealone
Name: "addtopath"; Description: "{cm:TaskAddToPath}"; GroupDescription: "{cm:GroupOther}"; Flags: unchecked

[Files]
; Main Executable & Core Libraries
Source: "..\build\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion; Components: app
Source: "..\build\liblibreomr.dll"; DestDir: "{app}"; Flags: ignoreversion; Components: app
Source: "..\build\{#MyAppCliName}"; DestDir: "{app}"; Flags: ignoreversion; Components: cli

; MinGW 64-bit Runtime Libraries
Source: "C:\mingw64\bin\libgcc_s_seh-1.dll"; DestDir: "{app}"; Flags: ignoreversion; Components: app
Source: "C:\mingw64\bin\libstdc++-6.dll"; DestDir: "{app}"; Flags: ignoreversion; Components: app
Source: "C:\mingw64\bin\libwinpthread-1.dll"; DestDir: "{app}"; Flags: ignoreversion; Components: app

; Web UI Assets
Source: "..\app\ui\*"; DestDir: "{app}\ui"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: app
Source: "..\app\ui\*"; DestDir: "{app}\app\ui"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: app

; Organized Templates - Portuguese
Source: "..\templates\pt\A4_Padrao\*"; DestDir: "{app}\templates\pt\A4_Padrao"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: templates\pt\std
Source: "..\templates\pt\Meia_Folha_2em1\*"; DestDir: "{app}\templates\pt\Meia_Folha_2em1"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: templates\pt\2in1
Source: "..\templates\pt\Quarto_Folha_4em1\*"; DestDir: "{app}\templates\pt\Quarto_Folha_4em1"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: templates\pt\4in1
Source: "..\templates\pt\Concurso_Certo_Errado\*"; DestDir: "{app}\templates\pt\Concurso_Certo_Errado"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: templates\pt\ce

; Organized Templates - English
Source: "..\templates\en\Standard_A4\*"; DestDir: "{app}\templates\en\Standard_A4"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: templates\en\std
Source: "..\templates\en\Half_Sheet_2in1\*"; DestDir: "{app}\templates\en\Half_Sheet_2in1"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: templates\en\2in1
Source: "..\templates\en\Quarter_Sheet_4in1\*"; DestDir: "{app}\templates\en\Quarter_Sheet_4in1"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: templates\en\4in1
Source: "..\templates\en\Assessment_True_False\*"; DestDir: "{app}\templates\en\Assessment_True_False"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: templates\en\ce

; Organized Templates - Spanish
Source: "..\templates\es\A4_Estandar\*"; DestDir: "{app}\templates\es\A4_Estandar"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: templates\es\std
Source: "..\templates\es\Media_Hoja_2en1\*"; DestDir: "{app}\templates\es\Media_Hoja_2en1"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: templates\es\2in1
Source: "..\templates\es\Cuarto_Hoja_4en1\*"; DestDir: "{app}\templates\es\Cuarto_Hoja_4en1"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: templates\es\4in1
Source: "..\templates\es\Examen_Verdadero_Falso\*"; DestDir: "{app}\templates\es\Examen_Verdadero_Falso"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: templates\es\ce

; Organized Templates - French
Source: "..\templates\fr\A4_Standard\*"; DestDir: "{app}\templates\fr\A4_Standard"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: templates\fr\std
Source: "..\templates\fr\Demi_Feuille_2en1\*"; DestDir: "{app}\templates\fr\Demi_Feuille_2en1"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: templates\fr\2in1
Source: "..\templates\fr\Quart_Feuille_4en1\*"; DestDir: "{app}\templates\fr\Quart_Feuille_4en1"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: templates\fr\4in1
Source: "..\templates\fr\Concours_Vrai_Faux\*"; DestDir: "{app}\templates\fr\Concours_Vrai_Faux"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: templates\fr\ce

; Organized Templates - German
Source: "..\templates\de\A4_Standard\*"; DestDir: "{app}\templates\de\A4_Standard"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: templates\de\std
Source: "..\templates\de\Halbes_Blatt_2in1\*"; DestDir: "{app}\templates\de\Halbes_Blatt_2in1"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: templates\de\2in1
Source: "..\templates\de\Viertel_Blatt_4in1\*"; DestDir: "{app}\templates\de\Viertel_Blatt_4in1"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: templates\de\4in1
Source: "..\templates\de\Pruefung_Richtig_Falsch\*"; DestDir: "{app}\templates\de\Pruefung_Richtig_Falsch"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: templates\de\ce

; Organized Templates - Italian
Source: "..\templates\it\A4_Standard\*"; DestDir: "{app}\templates\it\A4_Standard"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: templates\it\std
Source: "..\templates\it\Mezzo_Foglio_2in1\*"; DestDir: "{app}\templates\it\Mezzo_Foglio_2in1"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: templates\it\2in1
Source: "..\templates\it\Quarto_Foglio_4in1\*"; DestDir: "{app}\templates\it\Quarto_Foglio_4in1"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: templates\it\4in1
Source: "..\templates\it\Concorso_Vero_Falso\*"; DestDir: "{app}\templates\it\Concorso_Vero_Falso"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: templates\it\ce

; Documentation & Metadata
Source: "..\LICENSE"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\RELEASE_NOTES.md"; DestDir: "{app}"; Flags: ignoreversion

[Registry]
; Context Menu on Image Files
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\image\shell\LibreOMR"; ValueType: string; ValueName: ""; ValueData: "{cm:ContextMenuFile}"; Tasks: contextmenu; Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\image\shell\LibreOMR"; ValueType: string; ValueName: "Icon"; ValueData: """{app}\{#MyAppExeName}"",0"; Tasks: contextmenu; Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\image\shell\LibreOMR\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Tasks: contextmenu; Flags: uninsdeletekey

; Context Menu on Directories (Right-click on a folder)
Root: HKA; Subkey: "Software\Classes\Directory\shell\LibreOMR"; ValueType: string; ValueName: ""; ValueData: "{cm:ContextMenuFolder}"; Tasks: contextmenu; Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\Directory\shell\LibreOMR"; ValueType: string; ValueName: "Icon"; ValueData: """{app}\{#MyAppExeName}"",0"; Tasks: contextmenu; Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\Directory\shell\LibreOMR\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Tasks: contextmenu; Flags: uninsdeletekey

; Context Menu inside Directory Background (Right-click on blank area inside an open folder)
Root: HKA; Subkey: "Software\Classes\Directory\Background\shell\LibreOMR"; ValueType: string; ValueName: ""; ValueData: "{cm:ContextMenuFolder}"; Tasks: contextmenu; Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\Directory\Background\shell\LibreOMR"; ValueType: string; ValueName: "Icon"; ValueData: """{app}\{#MyAppExeName}"",0"; Tasks: contextmenu; Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\Directory\Background\shell\LibreOMR\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%V"""; Tasks: contextmenu; Flags: uninsdeletekey

; Optional: Add CLI to PATH
Root: HKA; Subkey: "Environment"; ValueType: expandsz; ValueName: "Path"; ValueData: "{olddata};{app}"; Tasks: addtopath; Check: NeedsAddPath(ExpandConstant('{app}'))

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; IconFilename: "{app}\{#MyAppExeName}"; Components: app
Name: "{group}\LibreOMR CLI"; Filename: "{app}\{#MyAppCliName}"; Components: cli
Name: "{group}\{cm:ShortcutTemplates}"; Filename: "{app}\templates"; Components: templates
Name: "{group}\{cm:ProgramOnTheWeb,LibreOMR}"; Filename: "{#MyAppURL}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon; IconFilename: "{app}\{#MyAppExeName}"; Components: app

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent; Components: app

[Code]
var
  DownloadPage: TDownloadWizardPage;
  SetupAborted: Boolean;

const
  AppUninstallSubkey = 'Software\Microsoft\Windows\CurrentVersion\Uninstall\{7E1C8302-3B44-4F90-8E15-927E26B64FA8}_is1';

function GetExistingUninstallString: String;
var
  RawPath: String;
begin
  Result := '';
  if RegQueryStringValue(HKCU, AppUninstallSubkey, 'UninstallString', RawPath) then
  begin
    Result := RemoveQuotes(RawPath);
    if FileExists(Result) then Exit;
  end;
  if RegQueryStringValue(HKLM, AppUninstallSubkey, 'UninstallString', RawPath) then
  begin
    Result := RemoveQuotes(RawPath);
    if FileExists(Result) then Exit;
  end;
  if RegQueryStringValue(HKLM64, AppUninstallSubkey, 'UninstallString', RawPath) then
  begin
    Result := RemoveQuotes(RawPath);
    if FileExists(Result) then Exit;
  end;
  Result := '';
end;

function GetExistingInstallDir: String;
var
  Dir: String;
begin
  Result := '';
  if RegQueryStringValue(HKCU, AppUninstallSubkey, 'InstallLocation', Dir) then
  begin
    if DirExists(Dir) then begin Result := Dir; Exit; end;
  end;
  if RegQueryStringValue(HKLM, AppUninstallSubkey, 'InstallLocation', Dir) then
  begin
    if DirExists(Dir) then begin Result := Dir; Exit; end;
  end;
  if RegQueryStringValue(HKLM64, AppUninstallSubkey, 'InstallLocation', Dir) then
  begin
    if DirExists(Dir) then begin Result := Dir; Exit; end;
  end;
end;

function ShouldSkipPage(PageID: Integer): Boolean;
begin
  Result := SetupAborted;
end;

procedure CheckPreviousInstallation;
var
  UninstPath: String;
  InstallDir: String;
  Buttons: TArrayOfString;
  Choice: Integer;
  ResultCode: Integer;
begin
  if WizardSilent then
    Exit;

  UninstPath := GetExistingUninstallString;
  if UninstPath <> '' then
  begin
    InstallDir := GetExistingInstallDir;
    SetArrayLength(Buttons, 2);
    Buttons[0] := CustomMessage('AlreadyInstalledModify') + #10 + CustomMessage('AlreadyInstalledModifySub');
    Buttons[1] := CustomMessage('AlreadyInstalledUninstall') + #10 + CustomMessage('AlreadyInstalledUninstallSub');

    Choice := TaskDialogMsgBox(
      CustomMessage('AlreadyInstalledTitle'),
      CustomMessage('AlreadyInstalledDesc'),
      mbInformation,
      MB_YESNOCANCEL,
      Buttons,
      0
    );

    if Choice = IDYES then
    begin
      // Modify / Update: prefill directory if available and continue setup
      if InstallDir <> '' then
        WizardForm.DirEdit.Text := InstallDir;
    end
    else if Choice = IDNO then
    begin
      // Uninstall: run existing uninstaller and exit setup
      SetupAborted := True;
      Exec(UninstPath, '', '', SW_SHOWNORMAL, ewWaitUntilTerminated, ResultCode);
      WizardForm.Close;
    end
    else
    begin
      // Cancelled or dialog closed: exit setup
      SetupAborted := True;
      WizardForm.Close;
    end;
  end;
end;

function IsWebView2Installed: Boolean;
var
  Version: String;
begin
  Result := False;
  // Check 64-bit machine registry
  if RegQueryStringValue(HKLM, 'SOFTWARE\WOW6432Node\Microsoft\EdgeUpdate\Clients\{F3017226-FE2A-4295-8BDF-6F3A4A3DE82F}', 'pv', Version) then
  begin
    if (Version <> '') and (Version <> '0.0.0.0') then
      Result := True;
  end;
  // Check 32-bit machine registry
  if not Result and RegQueryStringValue(HKLM, 'SOFTWARE\Microsoft\EdgeUpdate\Clients\{F3017226-FE2A-4295-8BDF-6F3A4A3DE82F}', 'pv', Version) then
  begin
    if (Version <> '') and (Version <> '0.0.0.0') then
      Result := True;
  end;
  // Check current user registry
  if not Result and RegQueryStringValue(HKCU, 'Software\Microsoft\EdgeUpdate\Clients\{F3017226-FE2A-4295-8BDF-6F3A4A3DE82F}', 'pv', Version) then
  begin
    if (Version <> '') and (Version <> '0.0.0.0') then
      Result := True;
  end;
end;

procedure InitializeWizard;
begin
  CheckPreviousInstallation;
  if SetupAborted then
    Exit;

  DownloadPage := CreateDownloadPage(CustomMessage('WebView2Title'), CustomMessage('WebView2Desc'), nil);
  DownloadPage.ShowBaseNameInsteadOfUrl := True;
end;

function NextButtonClick(CurPageID: Integer): Boolean;
var
  ResultCode: Integer;
  InstallerPath: String;
begin
  Result := True;
  if CurPageID = wpReady then
  begin
    if not IsWebView2Installed then
    begin
      DownloadPage.Clear;
      DownloadPage.Add('https://go.microsoft.com/fwlink/p/?LinkId=2124703', 'MicrosoftEdgeWebview2Setup.exe', '');
      DownloadPage.Show;
      try
        try
          DownloadPage.Download;
          InstallerPath := ExpandConstant('{tmp}\MicrosoftEdgeWebview2Setup.exe');
          WizardForm.StatusLabel.Caption := CustomMessage('WebView2Installing');
          Exec(InstallerPath, '/silent /install', '', SW_SHOW, ewWaitUntilTerminated, ResultCode);
        except
          MsgBox(CustomMessage('WebView2Error'), mbInformation, MB_OK);
        end;
      finally
        DownloadPage.Hide;
      end;
    end;
  end;
end;

function NeedsAddPath(Param: string): boolean;
var
  OrigPath: string;
begin
  if not RegQueryStringValue(HKEY_CURRENT_USER, 'Environment', 'Path', OrigPath) then
  begin
    Result := True;
    exit;
  end;
  Result := Pos(';' + UpperCase(Param) + ';', ';' + UpperCase(OrigPath) + ';') = 0;
end;
