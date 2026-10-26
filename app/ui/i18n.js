


const I18N_DATA = {
  en: {
    appName: "LibreOMR",
    appTagline: "Optical Mark Recognition",


    navProcess: "Grade Sheet",
    navBatch: "Batch Grading",
    navTemplates: "Answer Key",
    navGenerator: "Generate Sheet",
    navSettings: "Settings",
    navHelp: "Help",


    panelSheetImage: "Sheet Image",
    panelBatchResults: "Batch Results",
    panelAnswerTable: "Answer Key & Weights",
    panelSheetConfig: "Sheet Settings",
    panelRecognitionParams: "Recognition Parameters",
    sliderLightMark: "Light marks (pencil)",
    sliderDarkMark: "Dark marks (pen)",
    statusReady: "Ready",
    noFileLoaded: "No file loaded",
    noFileSelected: "No file selected",
    statusSensitivity: "Sensitivity: 0.32",
    metricLocalReading: "Local processing",
    themeNameLight: "Light",
    themeNameDark: "Dark",


    weightLabel: "Weight",
    voidOption: "★ VOID",
    btnSetAllWeights: "Set Weight for All...",
    weightPrompt: "Enter default weight for all questions (e.g., 1.0 or 2.0):",
    statusVoid: "Void / Free Mark",
    colWeight: "Weight",
    colPoints: "Score",


    btnLoadStudentList: "Import Student Roster (CSV)",
    btnClearStudentList: "Clear Roster",
    lblNoStudentList: "No roster loaded (using file names)",
    studentsLoaded: "{count} students loaded from roster",
    colStudent: "Student",
    colEnrollment: "ID / Roll No",
    statusPassed: "Passed",
    statusFailed: "Failed",
    passThresholdLabel: "Passing Grade (%):",


    statsTitle: "Class Statistics",
    statsAverage: "Class Average",
    statsHighest: "Highest Score",
    statsLowest: "Lowest Score",
    statsPassRate: "Pass Rate",
    itemAnalysisTitle: "Question Performance Analysis",
    colAccuracyRate: "Success Rate",
    colDifficulty: "Difficulty",
    colMostMarked: "Distribution / Most Marked",
    diffEasy: "Easy",
    diffMedium: "Medium",
    diffHard: "Hard",


    btnPrintSheetPdf: "Print A4 Sheet / PDF",
    btnPrintBatchReportPdf: "Print Class Report (PDF)",
    btnPrintSingleReportPdf: "Print Student Score Slip (PDF)",
    examReportTitle: "Class Assessment Report",
    studentSlipTitle: "Individual Evaluation Report",


    setupTagline: "INITIAL SETUP",
    introOverline: "LIBREOMR",
    introTitle: "Initial Configuration",
    introDesc: "Configure your default preferences. You can change these options anytime in Settings.",
    onbSkip: "Skip Setup",
    captionProfile: "Language",
    captionTheme: "Appearance",
    captionFormat: "Format",
    captionReady: "Ready",

    step1Title: "Select Language",
    step1Desc: "Choose your preferred language for the interface.",

    stepThemeTitle: "Appearance & Image Source",
    stepThemeDesc: "Choose interface theme and how exam sheets are captured.",
    themeLight: "Light Theme",
    themeLightDesc: "Standard light desktop theme",
    themeDark: "Dark Theme",
    themeDarkDesc: "Dark desktop theme for low-light environments",
    captureMobile: "Phone Camera",
    captureMobileDesc: "Optimized for camera photos with variable lighting",
    captureScanner: "Flatbed Scanner",
    captureScannerDesc: "Optimized for flat document scans",

    step3FormatTitle: "Default Exam Format",
    step3FormatDesc: "Choose default question count and number of options.",
    fieldOptions: "Choices per Question",
    fieldQuestions: "Default Question Count",
    qSuffix: "Questions",
    optionsADTitle: "A to D",
    optionsADDesc: "4 choices per question",
    optionsAETitle: "A to E",
    optionsAEDesc: "5 choices per question",
    langEnDesc: "Default (International)",
    langPtDesc: "Português (Brasil)",
    langEsDesc: "Español",
    summaryQuestions: "Questions",
    summaryLanguage: "Language",
    summaryTheme: "Theme",
    summaryCapture: "Capture Mode",

    step4Title: "Setup Complete",
    step4Desc: "Your preferences have been saved. You are ready to start.",
    onbFinishWithTour: "View Tutorial",
    onbFinishDirect: "Open Program",
    btnSetupPrev: "Back",
    btnSetupNext: "Next",
    btnSetupFinish: "Finish",


    tourBadge: "STEP {current} OF {total}",
    tourTipBadge: "GUIDE",
    tourSkip: "Close",
    tourNext: "Next",
    tourPrev: "Back",
    tourFinish: "Done",
    tour1Title: "1. Load Answer Sheet",
    tour1Desc: "Drop or select an exam sheet photo or scan (PNG, JPG, BMP).",
    tour2Title: "2. Exam Format",
    tour2Desc: "Set the question count and choices per question to match your exam.",
    tour3Title: "3. Answer Key & Weights",
    tour3Desc: "Specify correct answers, weights, and void questions to evaluate scores.",
    tour4Title: "4. Sheet Generator & PDF",
    tour4Desc: "Generate blank printable answer sheets with alignment markers or print directly to A4 PDF.",
    tour5Title: "5. Batch Grading & Analytics",
    tour5Desc: "Process an entire folder of sheets, import roster CSV, and view class statistics.",
    tour6Title: "6. Help & Tutorial",
    tour6Desc: "Click this button whenever you want to reopen this guide.",
    reopenOnboardingBtn: "Run Setup Wizard",
    startTourBtn: "View Tutorial",


    examConfigHeader: "Exam Format",
    numQuestionsLabel: "Number of Questions",
    optionsFormatLabel: "Choices per Question",
    optionsAD: "A - D (4 choices)",
    optionsAE: "A - E (5 choices)",
    optionsAC: "A - C (3 choices)",
    optionsAF: "A - F (6 choices)",
    quickSelect: "Presets:",


    singleTitle: "Grade Answer Sheet",
    singleSubtitle: "Select or drop an exam sheet image to process.",
    dropAreaTitle: "Drop answer sheet image here",
    dropAreaSubtitle: "or click to browse file (PNG, JPG, BMP)",
    browseImageBtn: "Browse File...",
    processBtn: "Grade Sheet",
    processing: "Processing...",


    resultsHeader: "Evaluation Results",
    metricScore: "Score",
    metricAccuracy: "Accuracy",
    metricAnswered: "Answered",
    metricTime: "Processing Time",
    exportCsvBtn: "Export CSV",
    exportJsonBtn: "Export JSON",
    tabTable: "Question List",
    tabImage: "Sheet View",
    colQuestion: "Question",
    colSelected: "Marked",
    colCorrect: "Key",
    colStatus: "Status",
    colConfidence: "Confidence",
    statusCorrect: "Correct",
    statusWrong: "Incorrect",
    statusBlank: "Blank",
    statusAmbiguous: "Multiple Marks",


    batchTitle: "Batch Grading",
    batchSubtitle: "Grade multiple sheets, link student roster, and analyze question performance.",
    batchDropTitle: "Drop exam sheet files here",
    batchFilesSelected: "files selected",
    startBatchBtn: "Grade All",
    batchProgress: "Processing batch...",
    exportBatchCsv: "Export Results (CSV)",
    colFilename: "File",
    colTotalScore: "Score",
    colPercent: "%",


    genTitle: "Generate Printable Sheets",
    genSubtitle: "Create blank answer sheets formatted for printing.",
    genRenderBtn: "Save Sheet Image (PNG)",
    genSuccess: "Blank sheet generated and saved successfully.",
    sheetLangLabel: "Sheet Text Language",
    optLangAuto: "Same as application (Automatic)",
    optLangPt: "Portuguese (Português)",
    optLangEn: "English (US/UK)",
    optLangEs: "Spanish (Español)",


    tmplTitle: "Official Answer Key & Weights",
    tmplSubtitle: "Configure correct answers, point weights, or mark questions as void.",
    saveTemplateBtn: "Save Answer Key",
    keysSaved: "Answer key and weights saved successfully.",
    questionsLabel: "Questions",
    selectAnswer: "Select",
    voidAction: "Void",
    voidShort: "Void",
    voidTooltip: "Mark question as void (awards full credit to all students)",
    unvoidTooltip: "Restore question to normal grading",
    weightTooltip: "Question point weight",
    btnFillPattern: "Fill Pattern",
    btnClearAllVoid: "Clear Voids",
    answerKeyInstructions: "Click a bubble to set the correct answer. Click 'Void' to cancel a question and give full credit to all students.",


    settingsTitle: "Detection Settings",
    settingsSubtitle: "Adjust threshold and image alignment options.",
    settingFillThreshold: "Bubble Fill Threshold",
    settingFillDesc: "Lower values detect lighter marks; higher values require darker fills.",
    settingAutoOrient: "Auto-Rotate Image",
    settingAutoOrientDesc: "Detect and correct upside-down or sideways images.",
    settingShadowFix: "Shadow & Lighting Correction",
    settingShadowFixDesc: "Normalize uneven lighting and hand shadows.",
    saveSettingsBtn: "Save Settings",
    settingsSaved: "Settings saved successfully."
  },

  pt: {
    appName: "LibreOMR",
    appTagline: "Leitor de Gabaritos",


    navProcess: "Corrigir Prova",
    navBatch: "Correção em Lote",
    navTemplates: "Gabarito Oficial",
    navGenerator: "Gerar Folha",
    navSettings: "Configurações",
    navHelp: "Ajuda",


    panelSheetImage: "Imagem da Folha",
    panelBatchResults: "Resultados da Turma",
    panelAnswerTable: "Tabela de Respostas e Pesos",
    panelSheetConfig: "Configurações da Folha",
    panelRecognitionParams: "Parâmetros do Reconhecimento",
    sliderLightMark: "Marcações leves (lápis)",
    sliderDarkMark: "Marcações escuras (caneta)",
    statusReady: "Pronto",
    noFileLoaded: "Nenhum arquivo carregado",
    noFileSelected: "Nenhum arquivo selecionado",
    statusSensitivity: "Sensibilidade: 0.32",
    metricLocalReading: "Leitura local",
    themeNameLight: "Claro",
    themeNameDark: "Escuro",


    weightLabel: "Peso",
    voidOption: "★ ANULADA",
    btnSetAllWeights: "Definir Peso para Todas...",
    weightPrompt: "Digite o peso padrão para todas as questões (ex: 1.0 ou 2.0):",
    statusVoid: "Anulada / Ponto Livre",
    colWeight: "Peso",
    colPoints: "Pontos",


    btnLoadStudentList: "Importar Lista da Turma (CSV)",
    btnClearStudentList: "Remover Lista",
    lblNoStudentList: "Nenhuma lista importada (usando nomes dos arquivos)",
    studentsLoaded: "{count} alunos vinculados da lista",
    colStudent: "Aluno",
    colEnrollment: "Matrícula / Nº",
    statusPassed: "Aprovado",
    statusFailed: "Reprovado",
    passThresholdLabel: "Nota de Corte (%):",


    statsTitle: "Estatísticas da Turma",
    statsAverage: "Média da Turma",
    statsHighest: "Maior Nota",
    statsLowest: "Menor Nota",
    statsPassRate: "Taxa de Aprovação",
    itemAnalysisTitle: "Raio-X das Questões (Análise de Dificuldade)",
    colAccuracyRate: "Taxa de Acerto",
    colDifficulty: "Dificuldade",
    colMostMarked: "Distribuição / Mais Marcada",
    diffEasy: "Fácil",
    diffMedium: "Média",
    diffHard: "Difícil",


    btnPrintSheetPdf: "Imprimir Folha A4 / Salvar PDF",
    btnPrintBatchReportPdf: "Imprimir Relatório da Turma (PDF)",
    btnPrintSingleReportPdf: "Imprimir Boletim do Aluno (PDF)",
    examReportTitle: "Relatório Consolidado da Avaliação",
    studentSlipTitle: "Espelho de Correção Individual",


    setupTagline: "CONFIGURAÇÃO INICIAL",
    introOverline: "LIBREOMR",
    introTitle: "Assistente de Configuração",
    introDesc: "Defina os parâmetros padrão de uso. Você pode alterar tudo depois nas configurações.",
    onbSkip: "Pular",
    captionProfile: "Idioma",
    captionTheme: "Aparência",
    captionFormat: "Formato",
    captionReady: "Concluído",

    step1Title: "Selecione o Idioma",
    step1Desc: "Escolha o idioma principal da interface do programa.",

    stepThemeTitle: "Aparência e Origem das Imagens",
    stepThemeDesc: "Defina o tema visual e a fonte principal das imagens das provas.",
    themeLight: "Tema Claro",
    themeLightDesc: "Interface clássica com fundo claro",
    themeDark: "Tema Escuro",
    themeDarkDesc: "Interface escura para menor cansaço visual",
    captureMobile: "Fotos de Celular",
    captureMobileDesc: "Otimizado para fotos com variações de luz e ângulo",
    captureScanner: "Scanner de Mesa",
    captureScannerDesc: "Otimizado para documentos planos e alinhados",

    step3FormatTitle: "Formato Padrão da Prova",
    step3FormatDesc: "Selecione a quantidade de questões e alternativas mais usada.",
    fieldOptions: "Alternativas por Questão",
    fieldQuestions: "Quantidade Padrão de Questões",
    qSuffix: "Questões",
    optionsADTitle: "A até D",
    optionsADDesc: "4 alternativas por questão",
    optionsAETitle: "A até E",
    optionsAEDesc: "5 alternativas por questão",
    langEnDesc: "English (US)",
    langPtDesc: "Português (Brasil)",
    langEsDesc: "Español",
    summaryQuestions: "Questões",
    summaryLanguage: "Idioma",
    summaryTheme: "Tema",
    summaryCapture: "Origem",

    step4Title: "Configuração Concluída",
    step4Desc: "Suas preferências foram salvas no programa.",
    onbFinishWithTour: "Ver Tutorial Rápido",
    onbFinishDirect: "Abrir Programa",
    btnSetupPrev: "Voltar",
    btnSetupNext: "Avançar",
    btnSetupFinish: "Concluir",


    tourBadge: "PASSO {current} DE {total}",
    tourTipBadge: "DICA",
    tourSkip: "Fechar",
    tourNext: "Próximo",
    tourPrev: "Voltar",
    tourFinish: "Concluir",
    tour1Title: "1. Carregar Folha de Respostas",
    tour1Desc: "Arraste a foto ou clique para selecionar o arquivo da prova (PNG, JPG, BMP).",
    tour2Title: "2. Formato da Avaliação",
    tour2Desc: "Ajuste o número de questões (20, 40, 50, 100) e alternativas (A-D ou A-E).",
    tour3Title: "3. Gabarito e Pesos",
    tour3Desc: "Defina as respostas corretas, pesos por questão e marque questões anuladas.",
    tour4Title: "4. Gerar Folha & PDF",
    tour4Desc: "Crie folhas de respostas com marcadores prontas para imprimir em PNG ou PDF A4.",
    tour5Title: "5. Correção em Lote e Turma",
    tour5Desc: "Corrija as provas da turma inteira, importe lista de alunos e veja o Raio-X das questões.",
    tour6Title: "6. Ajuda do Programa",
    tour6Desc: "Clique neste botão sempre que quiser rever este guia passo a passo.",
    reopenOnboardingBtn: "Executar Assistente Inicial",
    startTourBtn: "Ver Tutorial",


    examConfigHeader: "Formato da Prova",
    numQuestionsLabel: "Quantidade de Questões",
    optionsFormatLabel: "Alternativas por Questão",
    optionsAD: "A - D (4 alternativas)",
    optionsAE: "A - E (5 alternativas)",
    optionsAC: "A - C (3 alternativas)",
    optionsAF: "A - F (6 alternativas)",
    quickSelect: "Atalhos:",


    singleTitle: "Corrigir Folha de Respostas",
    singleSubtitle: "Selecione ou arraste a imagem do gabarito para processar.",
    dropAreaTitle: "Arraste a imagem da folha aqui",
    dropAreaSubtitle: "ou clique para selecionar o arquivo (PNG, JPG, BMP)",
    browseImageBtn: "Selecionar Arquivo...",
    processBtn: "Corrigir Prova",
    processing: "Processando...",


    resultsHeader: "Resultado da Correção",
    metricScore: "Nota Total",
    metricAccuracy: "Aproveitamento",
    metricAnswered: "Respondidas",
    metricTime: "Tempo de Processamento",
    exportCsvBtn: "Exportar CSV",
    exportJsonBtn: "Exportar JSON",
    tabTable: "Tabela de Questões",
    tabImage: "Visual da Folha",
    colQuestion: "Questão",
    colSelected: "Marcada",
    colCorrect: "Gabarito",
    colStatus: "Situação",
    colConfidence: "Confiança",
    statusCorrect: "Certa",
    statusWrong: "Errada",
    statusBlank: "Em Branco",
    statusAmbiguous: "Dupla / Rasura",


    batchTitle: "Correção em Lote",
    batchSubtitle: "Corrija múltiplas provas simultaneamente, vincule à lista de alunos e analise a turma.",
    batchDropTitle: "Arraste os arquivos das provas aqui",
    batchFilesSelected: "arquivos selecionados",
    startBatchBtn: "Corrigir Todas",
    batchProgress: "Processando lote...",
    exportBatchCsv: "Exportar Planilha (CSV)",
    colFilename: "Arquivo",
    colTotalScore: "Nota",
    colPercent: "%",


    genTitle: "Gerar Folhas para Impressão",
    genSubtitle: "Crie folhas de gabarito prontas para impressão com marcadores de alinhamento.",
    genRenderBtn: "Salvar Imagem (PNG)",
    genSuccess: "Folha gerada e salva com sucesso.",
    sheetLangLabel: "Idioma dos Textos da Folha",
    optLangAuto: "Mesmo idioma do aplicativo (Automático)",
    optLangPt: "Português (Brasil)",
    optLangEn: "Inglês (English)",
    optLangEs: "Espanhol (Español)",


    tmplTitle: "Gabarito Oficial e Pesos",
    tmplSubtitle: "Defina as alternativas corretas, pesos individuais ou anule questões.",
    saveTemplateBtn: "Salvar Gabarito",
    keysSaved: "Gabarito e pesos salvos com sucesso.",
    questionsLabel: "Questões",
    selectAnswer: "Selecionar",
    voidAction: "Anular",
    voidShort: "Anulada",
    voidTooltip: "Anular questão (atribui pontuação integral a todos os alunos)",
    unvoidTooltip: "Restaurar correção normal da questão",
    weightTooltip: "Peso da questão em pontos",
    btnFillPattern: "Preencher Padrão",
    btnClearAllVoid: "Limpar Anulações",
    answerKeyInstructions: "Clique na bolha para definir a resposta correta. Use 'Anular' para questões canceladas (pontuam 100% para todos os alunos).",


    settingsTitle: "Configurações de Leitura",
    settingsSubtitle: "Ajustes de sensibilidade e alinhamento de imagem.",
    settingFillThreshold: "Sensibilidade de Preenchimento",
    settingFillDesc: "Valores menores detectam marcas de lápis mais claras; valores maiores exigem caneta escura.",
    settingAutoOrient: "Girar Imagem Automaticamente",
    settingAutoOrientDesc: "Detecta e corrige imagens invertidas ou de lado.",
    settingShadowFix: "Compensação de Luz e Sombras",
    settingShadowFixDesc: "Remove sombras de celulares ou mãos sobre a folha.",
    saveSettingsBtn: "Salvar Configurações",
    settingsSaved: "Configurações salvas com sucesso."
  },

  es: {
    appName: "LibreOMR",
    appTagline: "Calificador de Exámenes",


    navProcess: "Calificar Examen",
    navBatch: "Calificación en Lote",
    navTemplates: "Clave de Respuestas",
    navGenerator: "Generar Hoja",
    navSettings: "Configuración",
    navHelp: "Ayuda",


    panelSheetImage: "Imagen de la Hoja",
    panelBatchResults: "Resultados de la Clase",
    panelAnswerTable: "Tabla de Respuestas y Ponderación",
    panelSheetConfig: "Configuración de la Hoja",
    panelRecognitionParams: "Parámetros de Reconocimiento",
    sliderLightMark: "Marcas tenues (lápiz)",
    sliderDarkMark: "Marcas oscuras (tinta)",
    statusReady: "Listo",
    noFileLoaded: "Ningún archivo cargado",
    noFileSelected: "Ningún archivo seleccionado",
    statusSensitivity: "Sensibilidad: 0.32",
    metricLocalReading: "Lectura local",
    themeNameLight: "Claro",
    themeNameDark: "Oscuro",


    weightLabel: "Ponderación",
    voidOption: "★ ANULADA",
    btnSetAllWeights: "Fijar Peso para Todas...",
    weightPrompt: "Ingrese el peso predeterminado para todas las preguntas (ej: 1.0 o 2.0):",
    statusVoid: "Anulada / Punto Libre",
    colWeight: "Peso",
    colPoints: "Puntos",


    btnLoadStudentList: "Importar Lista de la Clase (CSV)",
    btnClearStudentList: "Limpiar Lista",
    lblNoStudentList: "Sin lista cargada (usando nombres de archivos)",
    studentsLoaded: "{count} estudiantes vinculados de la lista",
    colStudent: "Estudiante",
    colEnrollment: "Matrícula / Nº",
    statusPassed: "Aprobado",
    statusFailed: "Reprobado",
    passThresholdLabel: "Nota de Aprobación (%):",


    statsTitle: "Estadísticas del Grupo",
    statsAverage: "Promedio del Grupo",
    statsHighest: "Calificación Más Alta",
    statsLowest: "Calificación Más Baja",
    statsPassRate: "Tasa de Aprobación",
    itemAnalysisTitle: "Análisis de Dificultad por Pregunta",
    colAccuracyRate: "Tasa de Acierto",
    colDifficulty: "Dificultad",
    colMostMarked: "Distribución / Más Marcada",
    diffEasy: "Fácil",
    diffMedium: "Media",
    diffHard: "Difícil",


    btnPrintSheetPdf: "Imprimir Hoja A4 / Guardar PDF",
    btnPrintBatchReportPdf: "Imprimir Informe de la Clase (PDF)",
    btnPrintSingleReportPdf: "Imprimir Boleta del Estudiante (PDF)",
    examReportTitle: "Informe Consolidado de la Evaluación",
    studentSlipTitle: "Boleta de Calificación Individual",


    setupTagline: "CONFIGURACIÓN INICIAL",
    introOverline: "LIBREOMR",
    introTitle: "Asistente de Configuración",
    introDesc: "Defina las preferencias predeterminadas. Puede cambiarlas después en Configuración.",
    onbSkip: "Omitir",
    captionProfile: "Idioma",
    captionTheme: "Apariencia",
    captionFormat: "Formato",
    captionReady: "Listo",

    step1Title: "Seleccione el Idioma",
    step1Desc: "Elija el idioma principal para la interfaz del programa.",

    stepThemeTitle: "Apariencia y Origen de Imágenes",
    stepThemeDesc: "Seleccione el tema y el método de captura de exámenes.",
    themeLight: "Tema Claro",
    themeLightDesc: "Interfaz estándar con fondo claro",
    themeDark: "Tema Oscuro",
    themeDarkDesc: "Interfaz oscura para menor fatiga visual",
    captureMobile: "Fotos de Móvil",
    captureMobileDesc: "Optimizado para fotos con luz y ángulo variable",
    captureScanner: "Escáner de Mesa",
    captureScannerDesc: "Optimizado para documentos planos y alineados",

    step3FormatTitle: "Formato Predeterminado",
    step3FormatDesc: "Seleccione la cantidad habitual de preguntas y opciones.",
    fieldOptions: "Opciones por Pregunta",
    fieldQuestions: "Cantidad Predeterminada de Preguntas",
    qSuffix: "Preguntas",
    optionsADTitle: "A hasta D",
    optionsADDesc: "4 opciones por pregunta",
    optionsAETitle: "A hasta E",
    optionsAEDesc: "5 opciones por pregunta",
    langEnDesc: "English (US)",
    langPtDesc: "Português (Brasil)",
    langEsDesc: "Español",
    summaryQuestions: "Preguntas",
    summaryLanguage: "Idioma",
    summaryTheme: "Tema",
    summaryCapture: "Origen",

    step4Title: "Configuración Completada",
    step4Desc: "Las preferencias se han guardado correctamente.",
    onbFinishWithTour: "Ver Tutorial Rápido",
    onbFinishDirect: "Abrir Programa",
    btnSetupPrev: "Volver",
    btnSetupNext: "Siguiente",
    btnSetupFinish: "Finalizar",


    tourBadge: "PASO {current} DE {total}",
    tourTipBadge: "GUÍA",
    tourSkip: "Cerrar",
    tourNext: "Siguiente",
    tourPrev: "Volver",
    tourFinish: "Finalizar",
    tour1Title: "1. Cargar Hoja de Respuestas",
    tour1Desc: "Arrastre la foto o seleccione el archivo del examen (PNG, JPG, BMP).",
    tour2Title: "2. Formato del Examen",
    tour2Desc: "Ajuste el número de preguntas y opciones de la prueba.",
    tour3Title: "3. Clave y Ponderaciones",
    tour3Desc: "Defina las respuestas correctas, ponderaciones y preguntas anuladas.",
    tour4Title: "4. Generar Hojas & PDF",
    tour4Desc: "Genere hojas de respuestas imprimibles con marcas de alineación o PDF A4.",
    tour5Title: "5. Calificación en Lote y Estadísticas",
    tour5Desc: "Califique exámenes del grupo, importe lista de alumnos y analice las preguntas.",
    tour6Title: "6. Ayuda del Programa",
    tour6Desc: "Haga clic aquí cuando desee consultar nuevamente esta guía.",
    reopenOnboardingBtn: "Ejecutar Asistente",
    startTourBtn: "Ver Tutorial",


    examConfigHeader: "Formato del Examen",
    numQuestionsLabel: "Cantidad de Preguntas",
    optionsFormatLabel: "Opciones por Pregunta",
    optionsAD: "A - D (4 opciones)",
    optionsAE: "A - E (5 opciones)",
    optionsAC: "A - C (3 opciones)",
    optionsAF: "A - F (6 opciones)",
    quickSelect: "Atajos:",


    singleTitle: "Calificar Hoja de Respuestas",
    singleSubtitle: "Seleccione o arrastre la imagen del examen para procesar.",
    dropAreaTitle: "Arrastre la imagen de la hoja aquí",
    dropAreaSubtitle: "o haga clic para seleccionar archivo (PNG, JPG, BMP)",
    browseImageBtn: "Seleccionar Archivo...",
    processBtn: "Calificar Examen",
    processing: "Procesando...",


    resultsHeader: "Resultado del Examen",
    metricScore: "Calificación Total",
    metricAccuracy: "Aciertos",
    metricAnswered: "Respondidas",
    metricTime: "Tiempo de Procesamiento",
    exportCsvBtn: "Exportar CSV",
    exportJsonBtn: "Exportar JSON",
    tabTable: "Lista de Preguntas",
    tabImage: "Visual de la Hoja",
    colQuestion: "Pregunta",
    colSelected: "Marcada",
    colCorrect: "Clave",
    colStatus: "Estado",
    colConfidence: "Confianza",
    statusCorrect: "Correcta",
    statusWrong: "Incorrecta",
    statusBlank: "En Blanco",
    statusAmbiguous: "Marca Múltiple",


    batchTitle: "Calificación en Lote",
    batchSubtitle: "Califique múltiples exámenes, vincule con la lista de alumnos y analice preguntas.",
    batchDropTitle: "Arrastre los archivos aquí",
    batchFilesSelected: "archivos seleccionados",
    startBatchBtn: "Calificar Todos",
    batchProgress: "Procesando lote...",
    exportBatchCsv: "Exportar Resultados (CSV)",
    colFilename: "Archivo",
    colTotalScore: "Nota",
    colPercent: "%",


    genTitle: "Generar Hojas para Imprimir",
    genSubtitle: "Cree hojas de respuestas listas para imprimir con marcas de alineación.",
    genRenderBtn: "Guardar Imagen (PNG)",
    genSuccess: "Hoja generada y guardada con éxito.",
    sheetLangLabel: "Idioma de los Textos de la Hoja",
    optLangAuto: "Mismo idioma de la aplicación (Automático)",
    optLangPt: "Portugués (Português)",
    optLangEn: "Inglés (English)",
    optLangEs: "Español",


    tmplTitle: "Clave de Respuestas y Ponderación",
    tmplSubtitle: "Configure respuestas correctas, pesos individuales o anule preguntas.",
    saveTemplateBtn: "Guardar Clave",
    keysSaved: "Clave y ponderaciones guardadas con éxito.",
    questionsLabel: "Preguntas",
    selectAnswer: "Seleccionar",
    voidAction: "Anular",
    voidShort: "Anulada",
    voidTooltip: "Anular pregunta (otorga puntuación total a todos)",
    unvoidTooltip: "Restaurar corrección normal de la pregunta",
    weightTooltip: "Ponderación de la pregunta en puntos",
    btnFillPattern: "Rellenar Patrón",
    btnClearAllVoid: "Borrar Anulaciones",
    answerKeyInstructions: "Haga clic en la burbuja para definir la respuesta correcta. Use 'Anular' para preguntas canceladas (otorga puntuación total a todos).",


    settingsTitle: "Configuración de Detección",
    settingsSubtitle: "Ajustes de sensibilidad y alineación de imagen.",
    settingFillThreshold: "Sensibilidad de Detección",
    settingFillDesc: "Valores menores detectan lápiz tenue; valores mayores requieren tinta oscura.",
    settingAutoOrient: "Rotar Imagen Automáticamente",
    settingAutoOrientDesc: "Detecta y corrige fotos invertidas o de lado.",
    settingShadowFix: "Corrección de Sombras y Luz",
    settingShadowFixDesc: "Elimina sombras de teléfonos o manos sobre el papel.",
    saveSettingsBtn: "Guardar Configuración",
    settingsSaved: "Configuración guardada con éxito."
  }
};

let currentLang = localStorage.getItem("libreomr_lang") || "pt";

function t(key) {
  if (I18N_DATA[currentLang] && I18N_DATA[currentLang][key]) {
    return I18N_DATA[currentLang][key];
  }
  if (I18N_DATA.pt && I18N_DATA.pt[key]) {
    return I18N_DATA.pt[key];
  }
  if (I18N_DATA.en && I18N_DATA.en[key]) {
    return I18N_DATA.en[key];
  }
  return key;
}

function setLanguage(lang) {
  if (!I18N_DATA[lang]) lang = "pt";
  currentLang = lang;
  localStorage.setItem("libreomr_lang", lang);
  applyTranslations();
  if (window.onLanguageChanged) {
    window.onLanguageChanged(lang);
  }
}

function applyTranslations() {
  document.querySelectorAll("[data-i18n]").forEach(el => {
    const key = el.getAttribute("data-i18n");
    el.innerHTML = t(key);
  });
  document.querySelectorAll("[data-i18n-placeholder]").forEach(el => {
    const key = el.getAttribute("data-i18n-placeholder");
    el.setAttribute("placeholder", t(key));
  });
  document.querySelectorAll("[data-i18n-title]").forEach(el => {
    const key = el.getAttribute("data-i18n-title");
    el.setAttribute("title", t(key));
  });

  const langSelect = document.getElementById("langSelect");
  if (langSelect) {
    langSelect.value = currentLang;
  }


  const lblTheme = document.getElementById("lblTheme");
  if (lblTheme) {
    const isDark = (document.documentElement.getAttribute("data-theme") === "dark");
    lblTheme.textContent = isDark ? (t("themeNameDark") || "Escuro") : (t("themeNameLight") || "Claro");
  }


  if (typeof updateFormatDisplay === "function") {
    updateFormatDisplay();
  }
}
