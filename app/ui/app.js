
let activeTab = "tab-process";
let currentStep = 1;
let selectedSingleImagePath = "";
let batchImagePaths = [];
let lastEvaluationResult = null;
let overlayZoom = 1.0;

let currentQuestionsCount = 40;
let currentOptionsCount = 5;
let customAnswerKeys = {};
let customWeights = {};

let studentRoster = [];

let lastBatchStats = null;
let lastItemAnalysis = [];

document.addEventListener("DOMContentLoaded", () => {
  initTheme();
  applyTranslations();
  initNavigation();
  initDragAndDrop();
  initExamConfig();
  initGenerator();
  checkFirstTimeSetup();
});

window.onLanguageChanged = function() {
  renderAnswerKeysEditor();
  updateTotalPointsBadge();
};

function initNavigation() {
  document.querySelectorAll(".nav-item[data-tab]").forEach(item => {
    item.addEventListener("click", () => {
      const tabId = item.getAttribute("data-tab");
      switchTab(tabId);
    });
  });

  document.getElementById("btnBrowseSingle").addEventListener("click", async (e) => {
    e.stopPropagation();
    if (window.api_pick_image) {
      const path = await window.api_pick_image();
      if (path && path.length > 0) {
        setSingleImage(path);
      }
    } else {
      setSingleImage("scratch/folha_escaneada_40q.png");
    }
  });

  document.getElementById("btnProcessSingle").addEventListener("click", processSingleSheet);

  document.getElementById("btnBrowseBatch").addEventListener("click", async (e) => {
    e.stopPropagation();
    if (window.api_pick_images) {
      const filesJson = await window.api_pick_images();
      try {
        const files = JSON.parse(filesJson);
        if (Array.isArray(files) && files.length > 0) {
          batchImagePaths = files;
          document.getElementById("batchCount").textContent = files.length;
        }
      } catch (err) {}
    } else {
      batchImagePaths = ["scratch/folha_escaneada_40q.png", "scratch/folha_escaneada_100q.png"];
      document.getElementById("batchCount").textContent = batchImagePaths.length;
    }
  });

  document.getElementById("btnRunBatch").addEventListener("click", runBatchEvaluation);
  document.getElementById("btnExportCsv").addEventListener("click", exportResultsCsv);
  document.getElementById("btnExportJson").addEventListener("click", exportResultsJson);
  document.getElementById("btnExportBatchCsv").addEventListener("click", exportBatchCsv);
  document.getElementById("btnSaveSettings").addEventListener("click", () => {
    alert(t("settingsSaved"));
  });
}

function switchTab(tabId) {
  activeTab = tabId;
  document.querySelectorAll(".nav-item[data-tab]").forEach(item => {
    item.classList.toggle("active", item.getAttribute("data-tab") === tabId);
  });
  document.querySelectorAll(".view-section").forEach(sec => {
    sec.classList.toggle("active", sec.id === tabId);
  });

  if (tabId === "tab-templates") {
    renderAnswerKeysEditor();
  }
}

function initExamConfig() {
  const inpQ = document.getElementById("cfgQuestions");
  const selOpt = document.getElementById("cfgOptions");

  const updateFormat = () => {
    currentQuestionsCount = parseInt(inpQ.value) || 40;
    currentOptionsCount = parseInt(selOpt.value) || 5;

    if (currentQuestionsCount < 1) currentQuestionsCount = 1;
    if (currentQuestionsCount > 100) currentQuestionsCount = 100;

    const optLabel = currentOptionsCount === 4 ? "A-D" : (currentOptionsCount === 3 ? "A-C" : (currentOptionsCount === 6 ? "A-F" : "A-E"));
    const qWord = t("summaryQuestions") || "Questões";
    const label = `${currentQuestionsCount} ${qWord} (${optLabel})`;
    const activeFmt = document.getElementById("lblActiveFormat");
    if (activeFmt) activeFmt.textContent = label;
    const tmplSum = document.getElementById("tmplStatusSummary");
    if (tmplSum) tmplSum.textContent = label;
    const sbFmt = document.getElementById("statusBarFormat");
    if (sbFmt) sbFmt.textContent = `${currentQuestionsCount} Q · ${optLabel}`;

    const genQ = document.getElementById("genQuestions");
    if (genQ) genQ.value = currentQuestionsCount;
    const genO = document.getElementById("genOptions");
    if (genO) genO.value = currentOptionsCount;

    document.querySelectorAll(".btn-preset").forEach(btn => {
      btn.classList.toggle("active", parseInt(btn.textContent) === currentQuestionsCount);
    });

    ensureAnswerKeys();
  };

  window.updateFormatDisplay = updateFormat;

  inpQ.addEventListener("change", updateFormat);
  inpQ.addEventListener("input", updateFormat);
  selOpt.addEventListener("change", updateFormat);

  updateFormat();
}

function setPresetQuestions(num) {
  document.getElementById("cfgQuestions").value = num;
  document.getElementById("cfgQuestions").dispatchEvent(new Event("change"));
}

function getLetterChoices() {
  const letters = ["A", "B", "C", "D", "E", "F"];
  return letters.slice(0, currentOptionsCount);
}

function ensureAnswerKeys() {
  const choices = getLetterChoices();
  for (let i = 1; i <= currentQuestionsCount; ++i) {
    const qKey = `q${i}`;
    if (!customAnswerKeys[qKey]) {
      customAnswerKeys[qKey] = choices[(i - 1) % choices.length];
    }
    if (customWeights[qKey] === undefined || customWeights[qKey] === null) {
      customWeights[qKey] = 1.0;
    }
  }
  updateTotalPointsBadge();
}

function updateTotalPointsBadge() {
  let total = 0;
  for (let i = 1; i <= currentQuestionsCount; ++i) {
    const qKey = `q${i}`;
    total += parseFloat(customWeights[qKey]) || 1.0;
  }
  const badge = document.getElementById("lblTotalMaxPoints");
  if (badge) {
    badge.textContent = `Total: ${total.toFixed(1)} pts`;
  }
}

function promptSetAllWeights() {
  const valStr = prompt(t("weightPrompt") || "Digite o peso padrão para todas as questões (ex: 1.0 ou 2.0):", "1.0");
  if (valStr !== null) {
    const val = parseFloat(valStr);
    if (!isNaN(val) && val > 0) {
      for (let i = 1; i <= currentQuestionsCount; ++i) {
        customWeights[`q${i}`] = val;
      }
      renderAnswerKeysEditor();
      updateTotalPointsBadge();
    }
  }
}

function fillAlternatingPattern() {
  const choices = getLetterChoices();
  for (let i = 1; i <= currentQuestionsCount; ++i) {
    const qKey = `q${i}`;
    customAnswerKeys[qKey] = choices[(i - 1) % choices.length];
  }
  renderAnswerKeysEditor();
}

function clearAllVoids() {
  const choices = getLetterChoices();
  for (let i = 1; i <= currentQuestionsCount; ++i) {
    const qKey = `q${i}`;
    if (customAnswerKeys[qKey] === "ANULADA") {
      customAnswerKeys[qKey] = choices[(i - 1) % choices.length];
    }
  }
  renderAnswerKeysEditor();
}

function renderAnswerKeysEditor() {
  ensureAnswerKeys();
  const container = document.getElementById("answerKeyInputs");
  if (!container) return;
  container.innerHTML = "";

  const choices = getLetterChoices();
  const count = currentQuestionsCount || 40;

  const itemsPerCol = count <= 40 ? 10 : (count <= 60 ? 15 : 20);
  const totalCols = Math.ceil(count / itemsPerCol);

  const board = document.createElement("div");
  board.className = "answer-sheet-board";

  for (let c = 0; c < totalCols; c++) {
    const startQ = c * itemsPerCol + 1;
    const endQ = Math.min((c + 1) * itemsPerCol, count);
    if (startQ > count) break;

    const col = document.createElement("div");
    col.className = "sheet-column";

    const colHeader = document.createElement("div");
    colHeader.className = "sheet-col-header";
    colHeader.innerHTML = `
      <span class="sheet-col-range">${t("questionsLabel") || "Questões"} ${String(startQ).padStart(2, '0')} - ${String(endQ).padStart(2, '0')}</span>
      <span class="sheet-col-meta">${choices.join(" · ")}</span>
    `;
    col.appendChild(colHeader);

    const rowsList = document.createElement("div");
    rowsList.className = "sheet-rows-list";

    for (let i = startQ; i <= endQ; i++) {
      const qKey = `q${i}`;
      const isVoid = (customAnswerKeys[qKey] === "ANULADA");
      const currentVal = customAnswerKeys[qKey] || choices[0];
      const weight = customWeights[qKey] !== undefined ? customWeights[qKey] : 1.0;

      const row = document.createElement("div");
      row.className = `sheet-row ${isVoid ? "is-void" : ""}`;
      row.id = `sheet-row-${qKey}`;

      let bubblesHtml = "";
      choices.forEach(ch => {
        const isSel = (!isVoid && currentVal === ch);
        bubblesHtml += `
          <button type="button"
                  class="sheet-bubble ${isSel ? 'active' : ''}"
                  data-q="${qKey}"
                  data-val="${ch}"
                  title="${t('selectAnswer') || 'Selecionar'} ${ch}">
            ${ch}
          </button>
        `;
      });

      row.innerHTML = `
        <span class="sheet-num">${String(i).padStart(2, '0')}</span>
        <div class="sheet-bubbles-group">
          ${bubblesHtml}
        </div>
        <button type="button"
                class="sheet-void-btn ${isVoid ? 'active' : ''}"
                data-q="${qKey}"
                title="${isVoid ? (t('unvoidTooltip') || 'Reativar questão') : (t('voidTooltip') || 'Anular questão')}">
          ${isVoid ? (t('voidShort') || 'Anulada') : (t('voidAction') || 'Anular')}
        </button>
        <div class="sheet-weight-control" title="${t('weightTooltip') || 'Peso da questão'}">
          <input type="number" step="0.5" min="0.1" max="100" class="sheet-weight-input" data-qw="${qKey}" value="${weight}">
          <span class="sheet-weight-unit">pt</span>
        </div>
      `;

      row.querySelectorAll(".sheet-bubble").forEach(btn => {
        btn.addEventListener("click", () => {
          const val = btn.getAttribute("data-val");
          customAnswerKeys[qKey] = val;
          row.classList.remove("is-void");
          const voidBtn = row.querySelector(".sheet-void-btn");
          if (voidBtn) {
            voidBtn.classList.remove("active");
            voidBtn.textContent = t("voidAction") || "Anular";
            voidBtn.title = t("voidTooltip") || "Anular questão";
          }
          row.querySelectorAll(".sheet-bubble").forEach(b => b.classList.remove("active"));
          btn.classList.add("active");
        });
      });

      const voidBtn = row.querySelector(".sheet-void-btn");
      if (voidBtn) {
        voidBtn.addEventListener("click", () => {
          const currentlyVoid = (customAnswerKeys[qKey] === "ANULADA");
          if (currentlyVoid) {

            customAnswerKeys[qKey] = choices[0];
            row.classList.remove("is-void");
            voidBtn.classList.remove("active");
            voidBtn.textContent = t("voidAction") || "Anular";
            voidBtn.title = t("voidTooltip") || "Anular questão";
            const bubble = row.querySelector(`.sheet-bubble[data-val="${customAnswerKeys[qKey]}"]`);
            if (bubble) bubble.classList.add("active");
          } else {
            customAnswerKeys[qKey] = "ANULADA";
            row.classList.add("is-void");
            voidBtn.classList.add("active");
            voidBtn.textContent = t("voidShort") || "Anulada";
            voidBtn.title = t("unvoidTooltip") || "Reativar questão";
            row.querySelectorAll(".sheet-bubble").forEach(b => b.classList.remove("active"));
          }
        });
      }

      const inpW = row.querySelector(".sheet-weight-input");
      if (inpW) {
        inpW.addEventListener("change", (e) => {
          const w = parseFloat(e.target.value) || 1.0;
          customWeights[qKey] = w;
          updateTotalPointsBadge();
        });
      }

      rowsList.appendChild(row);
    }

    col.appendChild(rowsList);
    board.appendChild(col);
  }

  container.appendChild(board);
  updateTotalPointsBadge();

  const btnSave = document.getElementById("btnSaveTemplate");
  if (btnSave) {
    btnSave.onclick = () => {
      alert(t("keysSaved"));
    };
  }
}

function evaluateStudentAnswers(rawQuestions) {
  ensureAnswerKeys();
  let totalScore = 0;
  let maxScore = 0;
  let correctCount = 0;
  let wrongCount = 0;
  let blankCount = 0;
  let ambiguousCount = 0;
  let answeredCount = 0;

  const evaluatedQuestions = rawQuestions.map((q, idx) => {
    const qKey = q.question_id || `q${idx + 1}`;
    const weight = parseFloat(customWeights[qKey]) || 1.0;
    const key = customAnswerKeys[qKey] || "A";
    const isVoid = (key === "ANULADA");

    maxScore += weight;

    let isCorrect = false;
    let earnedScore = 0;
    let status = "WRONG";

    if (isVoid) {
      isCorrect = true;
      earnedScore = weight;
      status = "VOID";
      correctCount++;
      if (q.is_answered) answeredCount++;
    } else if (!q.is_answered) {
      status = "BLANK";
      blankCount++;
    } else if (q.is_ambiguous) {
      status = "AMBIGUOUS";
      ambiguousCount++;
      answeredCount++;
    } else if (q.selected_answer === key) {
      isCorrect = true;
      earnedScore = weight;
      status = "CORRECT";
      correctCount++;
      answeredCount++;
    } else {
      status = "WRONG";
      wrongCount++;
      answeredCount++;
    }

    totalScore += earnedScore;

    return {
      ...q,
      question_id: qKey,
      correct_answer: key,
      weight: weight,
      earned_score: earnedScore,
      is_correct: isCorrect,
      status: status
    };
  });

  const scorePct = maxScore > 0 ? (totalScore / maxScore) * 100 : 0;
  const accuracyPct = (correctCount + wrongCount) > 0 ? (correctCount / (correctCount + wrongCount)) * 100 : 0;

  return {
    total_score: totalScore,
    max_score: maxScore,
    score_percentage: scorePct,
    accuracy_percentage: accuracyPct,
    correct_count: correctCount,
    wrong_count: wrongCount,
    blank_count: blankCount,
    ambiguous_count: ambiguousCount,
    answered_count: answeredCount,
    confidence: 0.96,
    questions: evaluatedQuestions
  };
}

function initDragAndDrop() {
  const dropzone = document.getElementById("sheetDropzone");
  if (!dropzone) return;

  dropzone.addEventListener("click", () => {
    document.getElementById("btnBrowseSingle").click();
  });

  ["dragenter", "dragover"].forEach(event => {
    dropzone.addEventListener(event, (e) => {
      e.preventDefault();
      e.stopPropagation();
      dropzone.classList.add("dragover");
    });
  });

  ["dragleave", "drop"].forEach(event => {
    dropzone.addEventListener(event, (e) => {
      e.preventDefault();
      e.stopPropagation();
      dropzone.classList.remove("dragover");
    });
  });

  dropzone.addEventListener("drop", (e) => {
    if (e.dataTransfer && e.dataTransfer.files && e.dataTransfer.files.length > 0) {
      const file = e.dataTransfer.files[0];
      setSingleImage(file.path || file.name);
    }
  });
}

function setSingleImage(filePath) {
  selectedSingleImagePath = filePath;
  const fileName = filePath.split(/[\\/]/).pop();
  const pill = document.getElementById("selectedFilePill");
  const pillText = document.getElementById("selectedFileName");
  if (pillText) pillText.textContent = fileName;
  if (pill) pill.classList.add("visible");
  const sbFile = document.getElementById("statusBarFile");
  if (sbFile) sbFile.textContent = fileName;
}

let currentTheme = localStorage.getItem("libreomr_theme") || "light";

function initTheme() {
  setTheme(currentTheme);
}

function setTheme(theme) {
  currentTheme = theme === "dark" ? "dark" : "light";
  document.documentElement.setAttribute("data-theme", currentTheme);
  localStorage.setItem("libreomr_theme", currentTheme);
  const lblTheme = document.getElementById("lblTheme");
  if (lblTheme) {
    lblTheme.textContent = currentTheme === "dark" ? (t("themeNameDark") || "Escuro") : (t("themeNameLight") || "Claro");
  }
}

function toggleTheme() {
  setTheme(currentTheme === "dark" ? "light" : "dark");
}

let onbCurrentStep = 1;
let onbSelectedTheme = currentTheme;
let onbSelectedCapture = "mobile";
const ONBOARDING_KEY = "libreomr_fullscreen_onboarding_v1";

function checkFirstTimeSetup() {
  const completed = localStorage.getItem(ONBOARDING_KEY);
  if (!completed) {
    openFullscreenOnboarding();
  }
}

function openFullscreenOnboarding() {
  onbCurrentStep = 1;
  const overlay = document.getElementById("fullscreenOnboarding");
  if (overlay) {
    overlay.classList.add("open");
    updateFullscreenOnboardingUI();
  }
}

function closeFullscreenOnboarding() {
  localStorage.setItem(ONBOARDING_KEY, "true");
  const overlay = document.getElementById("fullscreenOnboarding");
  if (overlay) {
    overlay.classList.remove("open");
  }
}

function skipFullscreenOnboarding() {
  closeFullscreenOnboarding();
}

function finishFullscreenOnboarding(startTour = false) {
  closeFullscreenOnboarding();
  if (startTour) {
    setTimeout(() => {
      startGuidedTutorial();
    }, 350);
  }
}

function onboardingGoToStep(stepNum) {
  if (stepNum >= 1 && stepNum <= 4) {
    onbCurrentStep = stepNum;
    updateFullscreenOnboardingUI();
  }
}

function onboardingNextStep() {
  if (onbCurrentStep < 4) {
    onbCurrentStep++;
    updateFullscreenOnboardingUI();
  } else {
    finishFullscreenOnboarding(false);
  }
}

function onboardingPrevStep() {
  if (onbCurrentStep > 1) {
    onbCurrentStep--;
    updateFullscreenOnboardingUI();
  }
}

function selectOnboardingLanguage(lang) {
  setLanguage(lang);
  updateFullscreenOnboardingUI();
}

function selectOnboardingTheme(theme) {
  onbSelectedTheme = theme;
  setTheme(theme);
  updateFullscreenOnboardingUI();
}

function selectOnboardingCapture(mode) {
  onbSelectedCapture = mode;
  const orientBox = document.getElementById("settingAutoOrient");
  const shadowBox = document.getElementById("settingAdaptiveThresh");
  if (mode === "mobile") {
    if (orientBox) orientBox.checked = true;
    if (shadowBox) shadowBox.checked = true;
  } else {
    if (orientBox) orientBox.checked = false;
    if (shadowBox) shadowBox.checked = false;
  }
  updateFullscreenOnboardingUI();
}

function selectOnboardingOptions(opts) {
  currentOptionsCount = opts;
  const sel = document.getElementById("cfgOptions");
  if (sel) {
    sel.value = opts;
    sel.dispatchEvent(new Event("change"));
  }
  updateFullscreenOnboardingUI();
}

function selectOnboardingQuestions(num) {
  currentQuestionsCount = num;
  const inp = document.getElementById("cfgQuestions");
  if (inp) {
    inp.value = num;
    inp.dispatchEvent(new Event("change"));
  }
  updateFullscreenOnboardingUI();
}

function updateFullscreenOnboardingUI() {
  document.querySelectorAll(".onb-step-panel").forEach(panel => {
    const s = parseInt(panel.getAttribute("data-onb-step"));
    panel.classList.toggle("active", s === onbCurrentStep);
  });

  document.querySelectorAll(".onb-step-pill").forEach(pill => {
    const s = parseInt(pill.getAttribute("data-pill-step"));
    pill.classList.toggle("active", s === onbCurrentStep);
    pill.classList.toggle("completed", s < onbCurrentStep);
  });

  const btnBack = document.getElementById("onbBtnBack");
  if (btnBack) {
    btnBack.style.visibility = onbCurrentStep === 1 ? "hidden" : "visible";
  }

  const nextLabel = document.getElementById("onbNextLabel");
  if (nextLabel) {
    nextLabel.textContent = onbCurrentStep === 4 ? (t("btnSetupFinish") || "Concluir") : (t("btnSetupNext") || "Avançar");
  }

  document.querySelectorAll(".onb-lang-card[data-lang]").forEach(card => {
    card.classList.toggle("selected", card.getAttribute("data-lang") === currentLang);
  });

  const cardLight = document.getElementById("onbThemeLightCard");
  const cardDark = document.getElementById("onbThemeDarkCard");
  if (cardLight && cardDark) {
    cardLight.classList.toggle("selected", currentTheme === "light");
    cardDark.classList.toggle("selected", currentTheme === "dark");
  }

  const cardPhone = document.getElementById("onbCapturePhoneCard");
  const cardScanner = document.getElementById("onbCaptureScannerCard");
  if (cardPhone && cardScanner) {
    cardPhone.classList.toggle("selected", onbSelectedCapture === "mobile");
    cardScanner.classList.toggle("selected", onbSelectedCapture === "scanner");
  }

  const opt4Card = document.getElementById("onbOpt4Card");
  const opt5Card = document.getElementById("onbOpt5Card");
  if (opt4Card && opt5Card) {
    opt4Card.classList.toggle("selected", currentOptionsCount === 4);
    opt5Card.classList.toggle("selected", currentOptionsCount === 5);
  }

  document.querySelectorAll(".onb-chips-grid .onb-q-chip").forEach(chip => {
    const q = parseInt(chip.getAttribute("data-q"));
    chip.classList.toggle("active", q === currentQuestionsCount);
  });

  const summaryLng = document.getElementById("onbSummaryLang");
  if (summaryLng) {
    const langNames = { en: "English (US)", pt: "Português (Brasil)", es: "Español" };
    summaryLng.textContent = langNames[currentLang] || "Português (Brasil)";
  }

  const summaryTheme = document.getElementById("onbSummaryTheme");
  if (summaryTheme) {
    summaryTheme.textContent = currentTheme === "dark" ? (t("themeDark") || "Tema Escuro") : (t("themeLight") || "Tema Claro");
  }

  const summaryFmt = document.getElementById("onbSummaryFormat");
  if (summaryFmt) {
    const optLabel = currentOptionsCount === 4 ? "A-D" : "A-E";
    summaryFmt.textContent = `${currentQuestionsCount} ${t("summaryQuestions") || "Questões"} (${optLabel})`;
  }

  const summaryCapture = document.getElementById("onbSummaryCapture");
  if (summaryCapture) {
    summaryCapture.textContent = onbSelectedCapture === "mobile" ? (t("captureMobile") || "Fotos de Celular") : (t("captureScanner") || "Scanner de Mesa");
  }
}

const TOUR_STEPS = [
  {
    targetSelector: "#sheetDropzone",
    fallbackTab: "tab-process",
    titleKey: "tour1Title",
    descKey: "tour1Desc",
    preferredPlacement: "below"
  },
  {
    targetSelector: ".exam-quick-config",
    fallbackTab: "tab-process",
    titleKey: "tour2Title",
    descKey: "tour2Desc",
    preferredPlacement: "below"
  },
  {
    targetSelector: '.nav-item[data-tab="tab-templates"]',
    fallbackTab: null,
    titleKey: "tour3Title",
    descKey: "tour3Desc",
    preferredPlacement: "right"
  },
  {
    targetSelector: '.nav-item[data-tab="tab-generator"]',
    fallbackTab: null,
    titleKey: "tour4Title",
    descKey: "tour4Desc",
    preferredPlacement: "right"
  },
  {
    targetSelector: '.nav-item[data-tab="tab-batch"]',
    fallbackTab: null,
    titleKey: "tour5Title",
    descKey: "tour5Desc",
    preferredPlacement: "right"
  },
  {
    targetSelector: "#btnHeaderTutorial",
    fallbackTab: null,
    titleKey: "tour6Title",
    descKey: "tour6Desc",
    preferredPlacement: "below-right"
  }
];

let tourCurrentIndex = 0;
let isTourRunning = false;

function startGuidedTutorial() {
  const onb = document.getElementById("fullscreenOnboarding");
  if (onb) onb.classList.remove("open");

  tourCurrentIndex = 0;
  isTourRunning = true;

  const overlay = document.getElementById("tutorialOverlay");
  if (overlay) {
    overlay.classList.add("active");
  }

  window.addEventListener("keydown", handleTourKeydown);
  window.addEventListener("resize", positionTourElements);

  renderTourStep();
}

function closeGuidedTutorial() {
  isTourRunning = false;
  const overlay = document.getElementById("tutorialOverlay");
  if (overlay) {
    overlay.classList.remove("active");
  }
  window.removeEventListener("keydown", handleTourKeydown);
  window.removeEventListener("resize", positionTourElements);
}

function handleTourKeydown(e) {
  if (!isTourRunning) return;
  if (e.key === "Escape") {
    closeGuidedTutorial();
  } else if (e.key === "ArrowRight" || e.key === "Enter") {
    nextTutorialStep();
  } else if (e.key === "ArrowLeft") {
    prevTutorialStep();
  }
}

function nextTutorialStep() {
  if (tourCurrentIndex < TOUR_STEPS.length - 1) {
    tourCurrentIndex++;
    renderTourStep();
  } else {
    closeGuidedTutorial();
  }
}

function prevTutorialStep() {
  if (tourCurrentIndex > 0) {
    tourCurrentIndex--;
    renderTourStep();
  }
}

function renderTourStep() {
  const step = TOUR_STEPS[tourCurrentIndex];
  if (!step) return;

  if (step.fallbackTab && activeTab !== step.fallbackTab) {
    switchTab(step.fallbackTab);
  }

  const stepBadge = document.getElementById("tutorialStepBadge");
  if (stepBadge) {
    const template = t("tourBadge") || "PASSO {current} DE {total}";
    stepBadge.textContent = template.replace("{current}", tourCurrentIndex + 1).replace("{total}", TOUR_STEPS.length);
  }

  const titleEl = document.getElementById("tutorialTitle");
  if (titleEl) {
    titleEl.textContent = t(step.titleKey);
  }

  const descEl = document.getElementById("tutorialDesc");
  if (descEl) {
    descEl.textContent = t(step.descKey);
  }

  const prevBtn = document.getElementById("tutorialBtnPrev");
  if (prevBtn) {
    prevBtn.style.visibility = tourCurrentIndex === 0 ? "hidden" : "visible";
  }

  const nextText = document.getElementById("tutorialNextText");
  if (nextText) {
    nextText.textContent = tourCurrentIndex === TOUR_STEPS.length - 1 ? (t("tourFinish") || "Concluir") : (t("tourNext") || "Próximo");
  }

  document.querySelectorAll("#tutorialDots .t-dot").forEach((dot, idx) => {
    dot.classList.toggle("active", idx === tourCurrentIndex);
  });

  requestAnimationFrame(() => {
    positionTourElements();
  });
}

function positionTourElements() {
  if (!isTourRunning) return;
  const step = TOUR_STEPS[tourCurrentIndex];
  if (!step) return;

  let targetEl = document.querySelector(step.targetSelector);
  if (!targetEl) {
    targetEl = document.querySelector(".top-bar") || document.body;
  }

  const rect = targetEl.getBoundingClientRect();
  const spotlight = document.getElementById("tutorialSpotlight");
  const popover = document.getElementById("tutorialPopover");
  const arrow = document.getElementById("tutorialArrow");

  if (!spotlight || !popover || !arrow) return;

  const pad = 6;
  spotlight.style.left = `${Math.max(0, rect.left - pad)}px`;
  spotlight.style.top = `${Math.max(0, rect.top - pad)}px`;
  spotlight.style.width = `${rect.width + pad * 2}px`;
  spotlight.style.height = `${rect.height + pad * 2}px`;

  const popoverWidth = 340;
  const popoverHeight = popover.offsetHeight || 200;
  const vw = window.innerWidth;
  const vh = window.innerHeight;

  let popLeft = 0;
  let popTop = 0;

  if (step.preferredPlacement === "right" || (rect.left < 260 && rect.right < 340)) {
    popLeft = rect.right + 20;
    popTop = Math.max(16, Math.min(vh - popoverHeight - 16, rect.top + rect.height / 2 - 60));
  } else if (step.preferredPlacement === "below-right") {
    popLeft = Math.max(16, rect.right - popoverWidth);
    popTop = rect.bottom + 16;
  } else {
    const spaceBelow = vh - rect.bottom;
    if (spaceBelow >= popoverHeight + 25) {
      popTop = rect.bottom + 16;
    } else {
      popTop = Math.max(16, rect.top - popoverHeight - 16);
    }
    popLeft = Math.max(16, Math.min(vw - popoverWidth - 16, rect.left + rect.width / 2 - popoverWidth / 2));
  }

  popover.style.left = `${popLeft}px`;
  popover.style.top = `${popTop}px`;
}

async function processSingleSheet() {
  if (!selectedSingleImagePath) {
    alert(t("dropAreaTitle"));
    return;
  }

  const btn = document.getElementById("btnProcessSingle");
  const originalText = btn.innerHTML;
  btn.innerHTML = `<span class="badge badge-neutral" style="margin-right:8px;">...</span> ${t("processing")}`;
  btn.disabled = true;

  const sbState = document.getElementById("statusBarState");
  if (sbState) sbState.textContent = t("processing");

  const payload = {
    image_path: selectedSingleImagePath,
    num_questions: currentQuestionsCount,
    num_options: currentOptionsCount,
    config: {
      fill_threshold: parseFloat(document.getElementById("settingThreshold").value) || 0.32,
      auto_align: true,
      auto_orient: document.getElementById("settingAutoOrient").checked,
      adaptive_thresholding: document.getElementById("settingAdaptiveThresh").checked,
      refine_bubble_centers: true
    }
  };

  try {
    const startTime = performance.now();
    let rawQuestions = [];

    if (window.api_process) {
      const resJson = await window.api_process(JSON.stringify(payload));
      const parsed = JSON.parse(resJson);
      rawQuestions = parsed.questions || [];
    } else {
      await new Promise(r => setTimeout(r, 80));
      const mockRaw = generateMockResult(currentQuestionsCount, currentOptionsCount);
      rawQuestions = mockRaw.questions || [];
    }

    const elapsed = Math.round(performance.now() - startTime);
    const result = evaluateStudentAnswers(rawQuestions);
    result.execution_time_ms = elapsed;
    lastEvaluationResult = result;

    renderResults(result);
  } catch (err) {
    alert("Error grading sheet: " + err.message);
  } finally {
    btn.innerHTML = originalText;
    btn.disabled = false;
    if (sbState) sbState.textContent = t("statusReady");
  }
}

function renderResults(result) {
  const sec = document.getElementById("resultsSection");
  sec.style.display = "block";
  sec.scrollIntoView({ behavior: "smooth" });

  document.getElementById("resScore").textContent = `${result.total_score.toFixed(1)} / ${result.max_score.toFixed(1)}`;
  document.getElementById("resScoreSub").textContent = `${result.score_percentage.toFixed(1)}% de aproveitamento`;
  document.getElementById("resAccuracy").textContent = `${result.accuracy_percentage.toFixed(1)}%`;
  document.getElementById("resAccuracySub").textContent = `${result.correct_count} ${t("statusCorrect")}, ${result.wrong_count} ${t("statusWrong")}`;
  document.getElementById("resAnswered").textContent = `${result.answered_count} / ${result.questions.length}`;
  document.getElementById("resAnsweredSub").textContent = `${result.blank_count} ${t("statusBlank")}`;
  document.getElementById("resTime").textContent = `${result.execution_time_ms} ms`;

  const tbody = document.getElementById("answersTableBody");
  tbody.innerHTML = "";

  result.questions.forEach(q => {
    const tr = document.createElement("tr");

    let statusBadge = "";
    if (q.status === "VOID") {
      statusBadge = `<span class="badge badge-warning">${t("statusVoid") || "Anulada"}</span>`;
    } else if (q.status === "BLANK") {
      statusBadge = `<span class="badge badge-neutral">${t("statusBlank")}</span>`;
    } else if (q.status === "AMBIGUOUS") {
      statusBadge = `<span class="badge badge-warning">${t("statusAmbiguous")}</span>`;
    } else if (q.status === "CORRECT") {
      statusBadge = `<span class="badge badge-success">${t("statusCorrect")}</span>`;
    } else {
      statusBadge = `<span class="badge badge-danger">${t("statusWrong")}</span>`;
    }

    const confPercent = Math.round((q.confidence || 0.95) * 100);
    const keyDisplay = (q.correct_answer === "ANULADA") ? `<span class="badge badge-warning" style="font-size:10px;">ANULADA</span>` : (q.correct_answer || "-");

    tr.innerHTML = `
      <td><strong>${q.question_id}</strong></td>
      <td><span style="font-size: 13.5px; font-weight: 700; color: var(--primary);">${q.selected_answer || "-"}</span></td>
      <td><span style="font-weight: 600;">${keyDisplay}</span></td>
      <td><span style="font-family: var(--font-mono); font-size: 11.5px;">${q.weight.toFixed(1)}</span></td>
      <td><strong style="font-family: var(--font-mono); font-size: 12px; color: ${q.is_correct ? 'var(--success)' : 'var(--text-muted)'};">${q.earned_score.toFixed(1)}</strong></td>
      <td>${statusBadge}</td>
      <td>
        <div style="display: flex; align-items: center; gap: 8px;">
          <div style="flex: 1; height: 6px; background: var(--bg-hover); border-radius: 3px; overflow: hidden;">
            <div style="width: ${confPercent}%; height: 100%; background: var(--primary); border-radius: 3px;"></div>
          </div>
          <span style="font-size: 11px; color: var(--text-dim); width: 32px;">${confPercent}%</span>
        </div>
      </td>
    `;
    tbody.appendChild(tr);
  });

  drawVisualOverlay(result);
}

function switchResultSubtab(type) {
  document.getElementById("tabBtnTable").classList.toggle("active", type === "table");
  document.getElementById("tabBtnImage").classList.toggle("active", type === "image");
  document.getElementById("subtabTable").style.display = (type === "table") ? "block" : "none";
  document.getElementById("subtabImage").style.display = (type === "image") ? "block" : "none";
}

function drawVisualOverlay(result) {
  const canvas = document.getElementById("overlayCanvas");
  if (!canvas) return;
  const ctx = canvas.getContext("2d");

  canvas.width = 800;
  canvas.height = 1100;

  ctx.fillStyle = "#ffffff";
  ctx.fillRect(0, 0, canvas.width, canvas.height);

  ctx.strokeStyle = "#cbd5e1";
  ctx.lineWidth = 1;
  ctx.strokeRect(40, 40, canvas.width - 80, canvas.height - 80);

  ctx.fillStyle = "#1d4ed8";
  const mSize = 24;
  ctx.fillRect(40, 40, mSize, mSize);
  ctx.fillRect(canvas.width - 40 - mSize, 40, mSize, mSize);
  ctx.fillRect(canvas.width - 40 - mSize, canvas.height - 40 - mSize, mSize, mSize);
  ctx.fillRect(40, canvas.height - 40 - mSize, mSize, mSize);

  let startY = 120;
  const total = result.questions.length;
  const cols = (total > 50) ? 4 : ((total > 20) ? 2 : 1);
  const questionsPerCol = Math.ceil(total / cols);
  const colWidth = (canvas.width - 140) / cols;
  const rowHeight = Math.min(36, (canvas.height - 220) / questionsPerCol);
  const letters = getLetterChoices();

  result.questions.forEach((q, idx) => {
    const col = Math.floor(idx / questionsPerCol);
    const row = idx % questionsPerCol;
    const qX = 70 + col * colWidth + 10;
    const qY = startY + row * rowHeight;

    ctx.font = "bold 11px sans-serif";
    ctx.fillStyle = "#0f172a";
    ctx.fillText(`${idx + 1}.`, qX, qY + 11);

    letters.forEach((l, lIdx) => {
      const bX = qX + 28 + lIdx * 20;
      const bY = qY + 7;

      ctx.beginPath();
      ctx.arc(bX, bY, 7, 0, 2 * Math.PI);

      if (q.selected_answer === l) {
        ctx.fillStyle = q.is_correct ? "#059669" : "#dc2626";
        ctx.fill();
        ctx.strokeStyle = q.is_correct ? "#047857" : "#b91c1c";
        ctx.stroke();

        ctx.font = "bold 9px sans-serif";
        ctx.fillStyle = "#ffffff";
        ctx.fillText(l, bX - 3.5, bY + 3);
      } else {
        ctx.fillStyle = "#f8fafc";
        ctx.fill();
        ctx.strokeStyle = (q.correct_answer === l || q.correct_answer === "ANULADA") ? "#059669" : "#94a3b8";
        ctx.lineWidth = (q.correct_answer === l || q.correct_answer === "ANULADA") ? 2 : 1;
        ctx.stroke();

        ctx.font = "9px sans-serif";
        ctx.fillStyle = (q.correct_answer === l || q.correct_answer === "ANULADA") ? "#059669" : "#64748b";
        ctx.fillText(l, bX - 3.5, bY + 3);
      }
    });
  });
}

function zoomOverlay(factor) {
  overlayZoom = Math.max(0.5, Math.min(3.0, overlayZoom * factor));
  const canvas = document.getElementById("overlayCanvas");
  if (canvas) {
    canvas.style.transform = `scale(${overlayZoom})`;
  }
}

function resetZoomOverlay() {
  overlayZoom = 1.0;
  const canvas = document.getElementById("overlayCanvas");
  if (canvas) {
    canvas.style.transform = "scale(1)";
  }
}

function handleRosterFileUpload(event) {
  const file = event.target.files && event.target.files[0];
  if (!file) return;

  const reader = new FileReader();
  reader.onload = (e) => {
    const text = e.target.result;
    parseStudentRosterCsv(text);
  };
  reader.readAsText(file, "UTF-8");
}

function parseStudentRosterCsv(csvText) {
  const lines = csvText.split(/\r?\n/).map(l => l.trim()).filter(l => l.length > 0);
  if (lines.length === 0) return;

  const firstLine = lines[0];
  const delim = firstLine.includes(";") ? ";" : (firstLine.includes("\t") ? "\t" : ",");

  studentRoster = [];
  let startIdx = 0;

  const fLower = firstLine.toLowerCase();
  if (fLower.includes("nome") || fLower.includes("name") || fLower.includes("aluno") || fLower.includes("student") || fLower.includes("matricula") || fLower.includes("id")) {
    startIdx = 1;
  }

  for (let i = startIdx; i < lines.length; ++i) {
    const cols = lines[i].split(delim).map(c => c.trim().replace(/^["']|["']$/g, ""));
    if (cols.length === 0 || !cols[0]) continue;

    let enrollment = "";
    let name = "";
    if (cols.length === 1) {
      enrollment = String(studentRoster.length + 1).padStart(2, "0");
      name = cols[0];
    } else {
      enrollment = cols[0];
      name = cols[1];
    }
    studentRoster.push({ enrollment, name });
  }

  updateRosterDisplay();
}

function updateRosterDisplay() {
  const lbl = document.getElementById("lblRosterStatus");
  const btnClear = document.getElementById("btnClearRoster");
  if (studentRoster.length > 0) {
    const msg = (t("studentsLoaded") || "{count} alunos vinculados da lista").replace("{count}", studentRoster.length);
    if (lbl) lbl.textContent = msg;
    if (btnClear) btnClear.style.display = "inline-flex";
  } else {
    if (lbl) lbl.textContent = t("lblNoStudentList") || "Nenhuma lista importada (usando nomes dos arquivos)";
    if (btnClear) btnClear.style.display = "none";
  }
}

function clearStudentRoster() {
  studentRoster = [];
  const inp = document.getElementById("inputRosterFile");
  if (inp) inp.value = "";
  updateRosterDisplay();
}

async function runBatchEvaluation() {
  if (batchImagePaths.length === 0) {
    alert("Please select files for batch evaluation.");
    return;
  }

  const btn = document.getElementById("btnRunBatch");
  btn.disabled = true;
  btn.textContent = t("batchProgress");

  const resultsCard = document.getElementById("batchResultsCard");
  resultsCard.style.display = "block";
  const tbody = document.getElementById("batchTableBody");
  tbody.innerHTML = "";

  const passThreshInp = document.getElementById("inputPassThreshold");
  const passThreshold = passThreshInp ? parseFloat(passThreshInp.value) || 60 : 60;

  const batchResults = [];

  for (let i = 0; i < batchImagePaths.length; ++i) {
    const file = batchImagePaths[i];
    const fileName = file.split(/[\\/]/).pop();

    let rawQuestions = [];
    if (window.api_process) {
      const payload = {
        image_path: file,
        num_questions: currentQuestionsCount,
        num_options: currentOptionsCount,
        config: { fill_threshold: 0.32, auto_align: true, auto_orient: true, adaptive_thresholding: true, refine_bubble_centers: true }
      };
      const resJson = await window.api_process(JSON.stringify(payload));
      const parsed = JSON.parse(resJson);
      rawQuestions = parsed.questions || [];
    } else {
      await new Promise(r => setTimeout(r, 40));
      const mockRaw = generateMockResult(currentQuestionsCount, currentOptionsCount);
      rawQuestions = mockRaw.questions || [];
    }

    const evalResult = evaluateStudentAnswers(rawQuestions);

    let enrollment = "";
    let studentName = "";

    if (studentRoster && studentRoster[i]) {
      enrollment = studentRoster[i].enrollment;
      studentName = studentRoster[i].name;
    } else {
      const cleanName = fileName.replace(/\.[^/.]+$/, "");
      const match = cleanName.match(/^([0-9]+)[_\-\s]+(.*)$/);
      if (match) {
        enrollment = match[1];
        studentName = match[2];
      } else {
        enrollment = String(i + 1).padStart(2, "0");
        studentName = cleanName;
      }
    }

    const isPassed = (evalResult.score_percentage >= passThreshold);

    evalResult.enrollment = enrollment;
    evalResult.student_name = studentName;
    evalResult.filename = fileName;
    evalResult.is_passed = isPassed;

    batchResults.push(evalResult);

    const tr = document.createElement("tr");
    tr.innerHTML = `
      <td><span style="font-family: var(--font-mono); font-size: 11.5px; color: var(--text-dim);">${enrollment}</span></td>
      <td><strong>${studentName}</strong></td>
      <td><span style="font-size: 11.5px; color: var(--text-muted);">${fileName}</span></td>
      <td><strong style="font-family: var(--font-mono);">${evalResult.total_score.toFixed(1)} / ${evalResult.max_score.toFixed(1)}</strong></td>
      <td><span style="font-family: var(--font-mono); font-weight: 600;">${evalResult.score_percentage.toFixed(1)}%</span></td>
      <td><span class="badge ${isPassed ? 'badge-success' : 'badge-danger'}">${isPassed ? (t("statusPassed") || "Aprovado") : (t("statusFailed") || "Reprovado")}</span></td>
      <td><span style="font-size: 11px; color: var(--text-dim);">${Math.round(evalResult.confidence * 100)}%</span></td>
    `;
    tbody.appendChild(tr);
  }

  btn.disabled = false;
  btn.textContent = t("startBatchBtn");
  document.getElementById("btnExportBatchCsv").style.display = "inline-flex";
  window.lastBatchResults = batchResults;

  computeAndRenderBatchStats(batchResults, passThreshold);
}

function computeAndRenderBatchStats(batchResults, passThreshold) {
  if (!batchResults || batchResults.length === 0) return;

  const totalStudents = batchResults.length;
  const scores = batchResults.map(r => r.total_score);
  const maxScores = batchResults.map(r => r.max_score);
  const examMax = maxScores[0] || 40.0;

  const avg = scores.reduce((a, b) => a + b, 0) / totalStudents;
  const avgPct = examMax > 0 ? (avg / examMax) * 100 : 0;

  let highScore = -Infinity;
  let highStudent = "";
  let lowScore = Infinity;
  let lowStudent = "";
  let passCount = 0;

  batchResults.forEach(r => {
    if (r.total_score > highScore) {
      highScore = r.total_score;
      highStudent = r.student_name;
    }
    if (r.total_score < lowScore) {
      lowScore = r.total_score;
      lowStudent = r.student_name;
    }
    if (r.is_passed) passCount++;
  });

  const passRate = (passCount / totalStudents) * 100;

  const elAvg = document.getElementById("statBatchAvg");
  const elAvgPct = document.getElementById("statBatchAvgPct");
  const elHigh = document.getElementById("statBatchHigh");
  const elHighStu = document.getElementById("statBatchHighStudent");
  const elLow = document.getElementById("statBatchLow");
  const elLowStu = document.getElementById("statBatchLowStudent");
  const elPassRate = document.getElementById("statBatchPassRate");
  const elPassCount = document.getElementById("statBatchPassCount");

  if (elAvg) elAvg.textContent = `${avg.toFixed(1)} / ${examMax.toFixed(1)}`;
  if (elAvgPct) elAvgPct.textContent = `${avgPct.toFixed(1)}% de média`;
  if (elHigh) elHigh.textContent = `${highScore.toFixed(1)}`;
  if (elHighStu) elHighStu.textContent = highStudent;
  if (elLow) elLow.textContent = `${lowScore.toFixed(1)}`;
  if (elLowStu) elLowStu.textContent = lowStudent;
  if (elPassRate) elPassRate.textContent = `${passRate.toFixed(1)}%`;
  if (elPassCount) elPassCount.textContent = `${passCount} / ${totalStudents} alunos aprovados`;

  const itemAnalysisTable = document.getElementById("itemAnalysisTableBody");
  if (!itemAnalysisTable) return;
  itemAnalysisTable.innerHTML = "";

  const itemAnalysis = [];

  for (let qIdx = 0; qIdx < currentQuestionsCount; ++qIdx) {
    const qKey = `q${qIdx + 1}`;
    const key = customAnswerKeys[qKey] || "A";
    const weight = parseFloat(customWeights[qKey]) || 1.0;
    const isVoid = (key === "ANULADA");

    let correctCount = 0;
    const choiceCounts = {};

    batchResults.forEach(r => {
      const q = r.questions && r.questions[qIdx];
      if (q) {
        if (q.is_correct) correctCount++;
        const marked = q.selected_answer || "Em branco";
        choiceCounts[marked] = (choiceCounts[marked] || 0) + 1;
      }
    });

    const accuracyRate = totalStudents > 0 ? (correctCount / totalStudents) * 100 : 0;
    let diffClass = "easy";
    let diffLabel = t("diffEasy") || "Fácil";
    if (accuracyRate < 40) {
      diffClass = "hard";
      diffLabel = t("diffHard") || "Difícil";
    } else if (accuracyRate < 70) {
      diffClass = "medium";
      diffLabel = t("diffMedium") || "Média";
    }

    let distStr = Object.entries(choiceCounts)
      .sort((a, b) => b[1] - a[1])
      .map(([k, v]) => `${k}: ${v} (${Math.round((v / totalStudents) * 100)}%)`)
      .slice(0, 3)
      .join(" &middot; ");

    itemAnalysis.push({
      question_id: qKey,
      key,
      weight,
      isVoid,
      correctCount,
      accuracyRate,
      diffClass,
      diffLabel,
      distStr
    });

    const tr = document.createElement("tr");
    tr.innerHTML = `
      <td><strong>${qKey}</strong></td>
      <td>${isVoid ? `<span class="badge badge-warning">ANULADA</span>` : `<strong>${key}</strong>`}</td>
      <td><span style="font-family: var(--font-mono); font-size: 11.5px;">${weight.toFixed(1)}</span></td>
      <td>
        <div class="diff-meter">
          <div class="diff-bar-track">
            <div class="diff-bar-fill ${diffClass}" style="width: ${accuracyRate}%;"></div>
          </div>
          <span style="font-family: var(--font-mono); font-size: 11.5px; width: 44px; text-align: right;">${accuracyRate.toFixed(0)}%</span>
        </div>
      </td>
      <td><span class="badge ${diffClass === 'easy' ? 'badge-success' : (diffClass === 'medium' ? 'badge-warning' : 'badge-danger')}">${diffLabel}</span></td>
      <td><span style="font-size: 11.5px; color: var(--text-dim);">${distStr}</span></td>
    `;
    itemAnalysisTable.appendChild(tr);
  }

  window.lastItemAnalysis = itemAnalysis;
}

function exportResultsCsv() {
  if (!lastEvaluationResult) return;
  let csv = "Question,Marked,Key,Weight,Score,Status,Confidence\n";
  lastEvaluationResult.questions.forEach(q => {
    csv += `"${q.question_id}","${q.selected_answer || ''}","${q.correct_answer || ''}",${q.weight.toFixed(1)},${q.earned_score.toFixed(1)},"${q.status}",${q.confidence.toFixed(3)}\n`;
  });
  saveTextFile("notas_gabarito.csv", csv);
}

function exportResultsJson() {
  if (!lastEvaluationResult) return;
  const json = JSON.stringify(lastEvaluationResult, null, 2);
  saveTextFile("notas_gabarito.json", json);
}

function exportBatchCsv() {
  if (!window.lastBatchResults) return;
  let csv = "Matricula,Aluno,Arquivo,Nota,NotaMaxima,Percentual,Situacao,Acertos,Erros,EmBranco\n";
  window.lastBatchResults.forEach(r => {
    const situacao = r.is_passed ? (t("statusPassed") || "Aprovado") : (t("statusFailed") || "Reprovado");
    csv += `"${r.enrollment}","${r.student_name}","${r.filename}",${r.total_score.toFixed(1)},${r.max_score.toFixed(1)},${r.score_percentage.toFixed(1)}%,"${situacao}",${r.correct_count},${r.wrong_count},${r.blank_count}\n`;
  });
  saveTextFile("notas_turma_completa.csv", csv);
}

async function saveTextFile(defaultName, content) {
  if (window.api_save_text_file && window.api_save_file) {
    const dest = await window.api_save_file(defaultName, defaultName.endsWith(".csv") ? "csv" : "json");
    if (dest && dest.length > 0) {
      await window.api_save_text_file(dest, content);
      alert("Saved: " + dest);
      return;
    }
  }
  const blob = new Blob([content], { type: "text/plain" });
  const a = document.createElement("a");
  a.href = URL.createObjectURL(blob);
  a.download = defaultName;
  a.click();
}

function initGenerator() {
  const btn = document.getElementById("btnGenerateSheet");
  if (!btn) return;

  btn.addEventListener("click", async () => {
    const qCount = parseInt(document.getElementById("genQuestions").value) || currentQuestionsCount;
    const optCount = parseInt(document.getElementById("genOptions").value) || currentOptionsCount;

    const langSel = document.getElementById("genLang");
    const langVal = langSel ? langSel.value : "auto";
    const sheetLang = (langVal === "auto") ? (localStorage.getItem("libreomr_lang") || "pt") : langVal;

    btn.disabled = true;
    btn.textContent = "Generating sheet...";

    if (window.api_generate_sheet) {
      const path = await window.api_generate_sheet(JSON.stringify({
        num_questions: qCount,
        num_options: optCount,
        dpi: 300,
        lang: sheetLang
      }));
      alert(t("genSuccess") + "\n" + path);
    } else {
      await new Promise(r => setTimeout(r, 100));
      alert(t("genSuccess") + ` (folha_${qCount}q_${optCount}alt_${sheetLang}.png)`);
    }

    btn.disabled = false;
    btn.textContent = t("genRenderBtn");
  });
}

function generateCanonicalSheetSvg(qCount, optCount, lang) {
  const letters = ["A", "B", "C", "D", "E", "F", "G", "H"].slice(0, optCount);

  const dpi = 300;
  const sheet_w = 2480;
  const sheet_h = 3508;
  const margin = dpi * 0.40;
  const marker_size = dpi * 0.20;
  const marker_half = Math.max(16, Math.floor(sheet_w / 75));
  const marker_dim = marker_half * 2;

  const left = margin + marker_size / 2.0;
  const top = margin + marker_size / 2.0;
  const right = sheet_w - margin - marker_size / 2.0;
  const bottom = sheet_h - margin - marker_size / 2.0;

  let cols = 1;
  if (qCount <= 25) cols = 1;
  else if (qCount <= 50) cols = 2;
  else if (qCount <= 100) cols = 4;
  else cols = 5;

  const questions_per_col = Math.ceil(qCount / cols);

  const start_y = dpi * 1.55;
  const bottom_y = sheet_h - margin - (dpi * 0.25);
  const available_h = bottom_y - start_y;

  let row_h = available_h / questions_per_col;
  const max_row_h = dpi * 0.26;
  if (row_h > max_row_h) row_h = max_row_h;

  let bubble_radius = Math.min(row_h * 0.26, dpi * 0.058);
  bubble_radius = Math.max(7.0, bubble_radius);

  const bubble_spacing = bubble_radius * 2.85;
  const col_span = (sheet_w - (margin * 2.0)) / cols;

  const label_pad = bubble_radius * 3.6;
  const block_width = label_pad + (optCount * bubble_spacing);

  const left_x = Math.round(left);
  const right_x = Math.round(right);
  const top_y = Math.round(top) + marker_half + 20;

  const TEXTS = {
    pt: {
      title: "FOLHA DE RESPOSTAS OFICIAL",
      subtitle: "CARTÃO-RESPOSTA PADRONIZADO PARA LEITURA ÓPTICA AUTOMATIZADA",
      studentName: "NOME DO ALUNO:",
      studentId: "MATRÍCULA / Nº:",
      date: "DATA: _____/_____/________",
      classRoom: "TURMA / SALA:",
      directions: "INSTRUÇÕES: Preencha completamente os círculos com caneta preta ou azul escuro. Marque apenas uma opção por questão. Não dobre nem rasure esta folha."
    },
    en: {
      title: "OFFICIAL EXAMINATION ANSWER SHEET",
      subtitle: "STANDARDIZED OPTICAL MARK RECOGNITION (OMR) ASSESSMENT FORM",
      studentName: "STUDENT NAME:",
      studentId: "STUDENT ID / ROLL NO:",
      date: "DATE: _____/_____/________",
      classRoom: "CLASS / ROOM:",
      directions: "DIRECTIONS: Fill in bubbles completely with dark ink or pencil. Mark one choice per question. Do not fold or smudge this sheet."
    },
    es: {
      title: "HOJA DE RESPUESTAS OFICIAL",
      subtitle: "PLANTILLA ESTANDARIZADA PARA LECTURA ÓPTICA (OMR)",
      studentName: "NOMBRE DEL ESTUDIANTE:",
      studentId: "MATRÍCULA / ID:",
      date: "FECHA: _____/_____/________",
      classRoom: "CURSO / SALA:",
      directions: "INSTRUCCIONES: Rellene completamente los círculos con bolígrafo negro o azul oscuro. Marque una sola opción por pregunta. No doble ni manche esta hoja."
    }
  };

  const tData = TEXTS[lang] || TEXTS.pt;

  let svg = `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${sheet_w} ${sheet_h}" width="210mm" height="297mm" style="background:#ffffff; display:block;">
    <rect width="${sheet_w}" height="${sheet_h}" fill="#ffffff"/>

    <rect x="${Math.round(left - marker_half)}" y="${Math.round(top - marker_half)}" width="${marker_dim}" height="${marker_dim}" fill="#000000"/>
    <rect x="${Math.round(right - marker_half)}" y="${Math.round(top - marker_half)}" width="${marker_dim}" height="${marker_dim}" fill="#000000"/>
    <rect x="${Math.round(right - marker_half)}" y="${Math.round(bottom - marker_half)}" width="${marker_dim}" height="${marker_dim}" fill="#000000"/>
    <rect x="${Math.round(left - marker_half)}" y="${Math.round(bottom - marker_half)}" width="${marker_dim}" height="${marker_dim}" fill="#000000"/>

    <line x1="${left_x}" y1="${top_y}" x2="${right_x}" y2="${top_y}" stroke="#000000" stroke-width="3"/>
    <line x1="${left_x}" y1="${top_y + 4}" x2="${right_x}" y2="${top_y + 4}" stroke="#000000" stroke-width="1"/>

    <text x="${left_x + 14}" y="${top_y + 40}" font-family="-apple-system, BlinkMacSystemFont, 'Segoe UI', Arial, sans-serif" font-size="34" font-weight="900" fill="#000000">${tData.title}</text>
    <text x="${left_x + 14}" y="${top_y + 68}" font-family="-apple-system, BlinkMacSystemFont, 'Segoe UI', Arial, sans-serif" font-size="16" font-weight="600" fill="#555555">${tData.subtitle} &middot; ${qCount} Q (${letters[0]}-${letters[letters.length-1]})</text>

    <rect x="${left_x}" y="${top_y + 86}" width="${right_x - left_x}" height="95" fill="none" stroke="#000000" stroke-width="2"/>
    <text x="${left_x + 20}" y="${top_y + 124}" font-family="-apple-system, BlinkMacSystemFont, 'Segoe UI', Arial, sans-serif" font-size="17" font-weight="700" fill="#000000">${tData.studentName} ___________________________________________   ${tData.date}</text>
    <text x="${left_x + 20}" y="${top_y + 160}" font-family="-apple-system, BlinkMacSystemFont, 'Segoe UI', Arial, sans-serif" font-size="17" font-weight="700" fill="#000000">${tData.studentId} ___________________________________________   ${tData.classRoom} _______________</text>

    <text x="${left_x + 8}" y="${top_y + 208}" font-family="-apple-system, BlinkMacSystemFont, 'Segoe UI', Arial, sans-serif" font-size="14.5" fill="#444444">${tData.directions}</text>
    <line x1="${left_x}" y1="${top_y + 224}" x2="${right_x}" y2="${top_y + 224}" stroke="#000000" stroke-width="2"/>
  `;

  if (cols > 1) {
    for (let c = 1; c < cols; ++c) {
      const divX = Math.round(margin + c * col_span);
      svg += `<line x1="${divX}" y1="${Math.round(start_y - 12)}" x2="${divX}" y2="${Math.round(bottom_y)}" stroke="#dddddd" stroke-width="1.5" stroke-dasharray="8,6"/>`;
    }
  }

  for (let q = 1; q <= qCount; ++q) {
    const c = Math.floor((q - 1) / questions_per_col);
    const r = (q - 1) % questions_per_col;

    const col_left = margin + (c * col_span);
    let offset_x = (col_span - block_width) * 0.5;
    if (offset_x < label_pad) offset_x = label_pad;

    const base_x = col_left + offset_x + (bubble_radius * 1.2);
    const base_y = start_y + (r * row_h) + (row_h * 0.5);

    const qNumStr = (q < 10 ? "0" : "") + q + ".";

    svg += `<text x="${(base_x - bubble_radius - 8).toFixed(1)}" y="${(base_y + bubble_radius * 0.38).toFixed(1)}" text-anchor="end" font-family="-apple-system, BlinkMacSystemFont, 'Segoe UI', Arial, sans-serif" font-size="${(bubble_radius * 1.15).toFixed(1)}" font-weight="bold" fill="#000000">${qNumStr}</text>`;

    for (let opt = 0; opt < optCount; ++opt) {
      const bx = base_x + (opt * bubble_spacing);
      const ch = letters[opt];

      svg += `<circle cx="${bx.toFixed(1)}" cy="${base_y.toFixed(1)}" r="${bubble_radius.toFixed(1)}" stroke="#000000" stroke-width="2" fill="none"/>`;

      svg += `<text x="${bx.toFixed(1)}" y="${(base_y + bubble_radius * 0.36).toFixed(1)}" text-anchor="middle" font-family="-apple-system, BlinkMacSystemFont, 'Segoe UI', Arial, sans-serif" font-size="${(bubble_radius * 1.05).toFixed(1)}" font-weight="bold" fill="#444444">${ch}</text>`;
    }
  }

  svg += `</svg>`;
  return svg;
}

function openPrintWindow(title, bodyHtml, isSheet = false) {
  const printWin = window.open("", "_blank", "width=850,height=900");
  if (!printWin) {
    alert("Por favor, permita pop-ups para imprimir o relatório.");
    return;
  }
  printWin.document.open();
  printWin.document.write(`
    <!DOCTYPE html>
    <html>
    <head>
      <meta charset="UTF-8">
      <title>${title}</title>
      <style>
        ${isSheet ? `
          @page { size: A4 portrait; margin: 0; }
          html, body {
            margin: 0;
            padding: 0;
            width: 210mm;
            height: 297mm;
            background: #ffffff;
            overflow: hidden;
            -webkit-print-color-adjust: exact;
            print-color-adjust: exact;
          }
          svg {
            width: 210mm;
            height: 297mm;
            display: block;
          }
        ` : `
          body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif; margin: 15mm 12mm; color: #111; font-size: 12px; line-height: 1.4; }
          h1, h2, h3, h4 { margin: 0 0 6px 0; color: #000; }
          .report-header { border-bottom: 2px solid #222; padding-bottom: 10px; margin-bottom: 14px; display: flex; justify-content: space-between; align-items: flex-end; }
          .meta-grid { display: grid; grid-template-columns: repeat(4, 1fr); gap: 10px; margin-bottom: 16px; }
          .meta-card { border: 1px solid #ccc; padding: 8px 10px; border-radius: 4px; }
          .meta-card small { display: block; font-size: 10px; text-transform: uppercase; color: #666; font-weight: 600; margin-bottom: 2px; }
          .meta-card strong { font-size: 16px; font-weight: 700; }
          table { width: 100%; border-collapse: collapse; margin-bottom: 16px; font-size: 11px; }
          th { background: #f0f0f0; border: 1px solid #ccc; padding: 6px 8px; text-align: left; }
          td { border: 1px solid #ddd; padding: 5px 8px; }
          .badge { display: inline-block; padding: 2px 6px; font-size: 10px; font-weight: 700; border-radius: 3px; }
          .badge-success { background: #d1e7dd; color: #0f5132; }
          .badge-danger { background: #f8d7da; color: #842029; }
          .badge-warning { background: #fff3cd; color: #664d03; }
          @media print {
            @page { size: A4 portrait; margin: 12mm; }
          }
        `}
      </style>
    </head>
    <body>
      ${bodyHtml}
      <script>
        window.onload = function() {
          setTimeout(function() { window.print(); }, 200);
        };
      <\/script>
    </body>
    </html>
  `);
  printWin.document.close();
}

function printBlankSheetA4() {
  const qCount = parseInt(document.getElementById("genQuestions").value) || currentQuestionsCount;
  const optCount = parseInt(document.getElementById("genOptions").value) || currentOptionsCount;

  const langSel = document.getElementById("genLang");
  const langVal = langSel ? langSel.value : "auto";
  const sheetLang = (langVal === "auto") ? (localStorage.getItem("libreomr_lang") || "pt") : langVal;

  const svg = generateCanonicalSheetSvg(qCount, optCount, sheetLang);
  const titles = {
    pt: "Folha de Gabarito A4 - LibreOMR (Português)",
    en: "A4 Answer Sheet - LibreOMR (English)",
    es: "Hoja de Respuestas A4 - LibreOMR (Español)"
  };
  const windowTitle = titles[sheetLang] || titles.pt;

  openPrintWindow(windowTitle, svg, true);
}

function printBatchClassReport() {
  if (!window.lastBatchResults || window.lastBatchResults.length === 0) {
    alert("Nenhum resultado de lote disponível para impressão.");
    return;
  }

  const results = window.lastBatchResults;
  const total = results.length;
  const examMax = results[0].max_score;
  const avg = results.reduce((a, b) => a + b.total_score, 0) / total;
  const avgPct = examMax > 0 ? (avg / examMax) * 100 : 0;
  const passCount = results.filter(r => r.is_passed).length;
  const passRate = (passCount / total) * 100;

  let rowsHtml = "";
  results.forEach(r => {
    rowsHtml += `
      <tr>
        <td>${r.enrollment}</td>
        <td><strong>${r.student_name}</strong></td>
        <td>${r.filename}</td>
        <td><strong>${r.total_score.toFixed(1)} / ${r.max_score.toFixed(1)}</strong></td>
        <td>${r.score_percentage.toFixed(1)}%</td>
        <td><span class="badge ${r.is_passed ? 'badge-success' : 'badge-danger'}">${r.is_passed ? 'Aprovado' : 'Reprovado'}</span></td>
        <td>${r.correct_count} certas, ${r.wrong_count} erradas, ${r.blank_count} em branco</td>
      </tr>
    `;
  });

  let analysisRowsHtml = "";
  if (window.lastItemAnalysis && window.lastItemAnalysis.length > 0) {
    window.lastItemAnalysis.forEach(item => {
      analysisRowsHtml += `
        <tr>
          <td><strong>${item.question_id}</strong></td>
          <td>${item.isVoid ? '<span class="badge badge-warning">ANULADA</span>' : item.key}</td>
          <td>${item.weight.toFixed(1)}</td>
          <td><strong>${item.accuracyRate.toFixed(1)}%</strong> (${item.correctCount}/${total})</td>
          <td><span class="badge ${item.diffClass === 'easy' ? 'badge-success' : (item.diffClass === 'medium' ? 'badge-warning' : 'badge-danger')}">${item.diffLabel}</span></td>
          <td>${item.distStr}</td>
        </tr>
      `;
    });
  }

  const html = `
    <div class="report-header">
      <div>
        <h1>Relatório Consolidado de Avaliação</h1>
        <p style="margin:2px 0; color:#555;">LibreOMR Desktop &middot; Correção Automática de Gabaritos</p>
      </div>
      <div style="text-align:right;">
        <strong>Total de Alunos: ${total}</strong><br>
        <small style="color:#666;">Data: ${new Date().toLocaleDateString()}</small>
      </div>
    </div>

    <div class="meta-grid">
      <div class="meta-card">
        <small>Média da Turma</small>
        <strong>${avg.toFixed(1)} / ${examMax.toFixed(1)}</strong>
        <div style="font-size:10px; color:#666;">${avgPct.toFixed(1)}% de aproveitamento</div>
      </div>
      <div class="meta-card">
        <small>Taxa de Aprovação</small>
        <strong>${passRate.toFixed(1)}%</strong>
        <div style="font-size:10px; color:#666;">${passCount} de ${total} alunos</div>
      </div>
      <div class="meta-card">
        <small>Gabarito</small>
        <strong>${currentQuestionsCount} Questões</strong>
        <div style="font-size:10px; color:#666;">Opções ${getLetterChoices()[0]}-${getLetterChoices()[getLetterChoices().length-1]}</div>
      </div>
      <div class="meta-card">
        <small>Pontuação Máxima</small>
        <strong>${examMax.toFixed(1)} pts</strong>
        <div style="font-size:10px; color:#666;">Com pesos ponderados</div>
      </div>
    </div>

    <h3>Classificação Geral da Turma</h3>
    <table>
      <thead>
        <tr>
          <th style="width: 70px;">Nº / Matr.</th>
          <th>Aluno</th>
          <th>Arquivo</th>
          <th style="width: 90px;">Nota</th>
          <th style="width: 70px;">%</th>
          <th style="width: 80px;">Situação</th>
          <th>Detalhamento</th>
        </tr>
      </thead>
      <tbody>
        ${rowsHtml}
      </tbody>
    </table>

    ${analysisRowsHtml ? `
      <h3 style="margin-top: 24px;">Raio-X das Questões (Análise de Desempenho e Distratores)</h3>
      <table>
        <thead>
          <tr>
            <th style="width: 65px;">Questão</th>
            <th style="width: 75px;">Gabarito</th>
            <th style="width: 55px;">Peso</th>
            <th style="width: 120px;">Taxa de Acerto</th>
            <th style="width: 75px;">Dificuldade</th>
            <th>Distribuição de Alternativas Marcadas</th>
          </tr>
        </thead>
        <tbody>
          ${analysisRowsHtml}
        </tbody>
      </table>
    ` : ''}
  `;

  openPrintWindow("Relatório da Turma - LibreOMR", html);
}

function printSingleStudentSlip() {
  if (!lastEvaluationResult) {
    alert("Nenhuma avaliação individual ativa para impressão.");
    return;
  }

  const res = lastEvaluationResult;
  const fileName = selectedSingleImagePath ? selectedSingleImagePath.split(/[\\/]/).pop() : "prova_aluno.png";

  let qRows = "";
  res.questions.forEach(q => {
    let statusBadge = "";
    if (q.status === "VOID") statusBadge = '<span class="badge badge-warning">ANULADA</span>';
    else if (q.status === "CORRECT") statusBadge = '<span class="badge badge-success">Certa</span>';
    else if (q.status === "BLANK") statusBadge = '<span class="badge badge-warning">Em Branco</span>';
    else statusBadge = '<span class="badge badge-danger">Errada</span>';

    qRows += `
      <tr>
        <td><strong>${q.question_id}</strong></td>
        <td><strong>${q.selected_answer || '-'}</strong></td>
        <td>${q.correct_answer === 'ANULADA' ? '<span class="badge badge-warning">ANULADA</span>' : q.correct_answer}</td>
        <td>${q.weight.toFixed(1)}</td>
        <td><strong>${q.earned_score.toFixed(1)}</strong></td>
        <td>${statusBadge}</td>
      </tr>
    `;
  });

  const html = `
    <div class="report-header">
      <div>
        <h1>Boletim de Avaliação do Aluno</h1>
        <p style="margin:2px 0; color:#555;">Espelho de Correção Automatizado &middot; LibreOMR</p>
      </div>
      <div style="text-align:right;">
        <strong>Arquivo: ${fileName}</strong><br>
        <small style="color:#666;">Data: ${new Date().toLocaleDateString()}</small>
      </div>
    </div>

    <div class="meta-grid">
      <div class="meta-card">
        <small>Nota Final</small>
        <strong>${res.total_score.toFixed(1)} / ${res.max_score.toFixed(1)}</strong>
        <div style="font-size:10px; color:#666;">${res.score_percentage.toFixed(1)}% de acerto</div>
      </div>
      <div class="meta-card">
        <small>Acertos</small>
        <strong>${res.correct_count}</strong>
        <div style="font-size:10px; color:#666;">de ${res.questions.length} questões</div>
      </div>
      <div class="meta-card">
        <small>Erros</small>
        <strong>${res.wrong_count}</strong>
        <div style="font-size:10px; color:#666;">questões incorretas</div>
      </div>
      <div class="meta-card">
        <small>Em Branco / Dupla</small>
        <strong>${res.blank_count}</strong>
        <div style="font-size:10px; color:#666;">sem resposta válida</div>
      </div>
    </div>

    <h3>Detalhamento Questão por Questão</h3>
    <table>
      <thead>
        <tr>
          <th style="width: 70px;">Questão</th>
          <th style="width: 90px;">Marcada</th>
          <th style="width: 90px;">Gabarito</th>
          <th style="width: 70px;">Peso</th>
          <th style="width: 70px;">Pontos</th>
          <th>Situação</th>
        </tr>
      </thead>
      <tbody>
        ${qRows}
      </tbody>
    </table>
  `;

  openPrintWindow("Boletim do Aluno - LibreOMR", html);
}

function generateMockResult(count, optCount) {
  const questions = [];
  const letters = ["A", "B", "C", "D", "E", "F"].slice(0, optCount);

  for (let i = 1; i <= count; ++i) {
    const key = customAnswerKeys[`q${i}`] || letters[(i - 1) % letters.length];
    const isAnswered = (i !== 7);
    const selected = isAnswered ? ((i % 8 === 0) ? letters[(i) % letters.length] : key) : "";

    questions.push({
      question_id: `q${i}`,
      question_name: `Question ${i}`,
      selected_answer: selected,
      correct_answer: key,
      is_answered: isAnswered,
      is_correct: false,
      is_ambiguous: false,
      confidence: 0.95 + (Math.random() * 0.04)
    });
  }

  return {
    alignment_successful: true,
    questions: questions
  };
}
