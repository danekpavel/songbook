#include <string>

namespace songbook {
   std::string latex_document_start = R"(\documentclass[a4paper,10pt]{article}

\usepackage{fontspec}
\usepackage{xunicode}
\usepackage{polyglossia}
\setmainlanguage{czech}
\setmainfont[Mapping=tex-text]{@@@mainFont@@@}
\setsansfont{@@@sansFont@@@}
\usepackage[width=18cm, height=26.5cm, top=1.5cm]{geometry}
\usepackage{multicol}
\usepackage[unicode, hidelinks]{hyperref}
\usepackage[titles]{tocloft}  % table of contents formatting
\usepackage{titlesec}         % section formatting

%%  Define labels
%   chorus
\newcommand{\choruslabel}{@@@chorusLabel@@@}
%   TOC
\newcommand{\toctitle}{@@@tocTitle@@@}

% sections and below won't be numbered
\setcounter{secnumdepth}{0}

%%  An invisible rule used to unify the height and depth of chord text
\newcommand{\chordstrutrule}{\vrule width0pt height0.9\baselineskip depth0.1\baselineskip}

%%  Puts a chord in a hbox together with the height/depth setting vrule
\newcommand{\chordsbox}[1]{\hbox{\chordstrutrule #1}}

%%  Places a chord above lyrics
\newcommand{\chordslyrics}[2]{\vbox{\chordsbox{#1}\hbox{#2}}}

%%  Boxes for measuring chord/lyrics widths
\newbox\chordhypbox
\newbox\lyricshypbox

%%  Places a chord(s) (#1) above lyrics (#2) ending with a hyphen (potentially with fills around) when necessary
\newcommand{\chordslyricshyphen}[2]{%
\setbox\chordhypbox=\chordsbox{#1}%
\setbox\lyricshypbox=\hbox{#2}%
\ifdim\wd\chordhypbox>\wd\lyricshypbox% chord is longer than lyrics
  \setbox\lyricshypbox=\hbox{#2-}%
  \ifdim\wd\chordhypbox>\wd\lyricshypbox% chord is longer than lyrics and hyphen
\vbox{\copy\chordhypbox\hbox to\wd\chordhypbox{#2\hfill-\hfill}}%
  \else\vbox{\copy\chordhypbox\hbox{#2-}}\fi%
\else\chordslyrics{#1}{#2}\fi
}

%%  One line of lyrics, chords or both
\newcommand{\sbline}[1]{\noindent #1\par}

%%  Verse
\renewcommand{\verse}[1]{#1\vskip\baselineskip}

%%  Measure chorus indent
\newlength\chorusindent
\setbox\chordhypbox=\hbox{\choruslabel\hskip1em}
\setlength{\chorusindent}{\wd\chordhypbox}

%%  Chorus
\newcommand{\chorus}[1]{{\leftskip=\chorusindent\parindent=-\chorusindent\indent\hbox to\chorusindent{\textit{\choruslabel}\hfill}#1\par\vskip\baselineskip}}

%% Section title format
\titleformat{\section}
  {\normalfont\LARGE}{}{0em}{}[\vskip2pt{\hrule width\textwidth height.4pt}]

%%  Starts a new song (#1) as a new section on a new page and puts song info below -- left (#2) and right (#3) justified parts
\newcommand{\song}[3]{\newpage\pagestyle{plain}\phantomsection\section{#1}  % \phantomsection necessary for hyperref to work
\vskip-3pt\noindent\textit{#2\hfill #3}\par\vskip2\baselineskip}

%%  Prints a chord with horizontal space after it
\newcommand{\chord}[1]{\textsf{#1}\hskip.4em}

%% Prints sharp symbol (#)
\newcommand\msharp{\raisebox{1pt}{$\sharp$}}

%%  Table of contents formatting
\renewcommand\cftsecfont{\normalfont}            % section (song) name font
\renewcommand\cftsecpagefont{\normalfont}        % page number font
\setlength\cftbeforesecskip{.2\baselineskip}     % vertical space between TOC entries

%%  Suppress automatic table of contents section header
\makeatletter
\renewcommand\tableofcontents{%
    \@starttoc{toc}%
}
\makeatother

%%  Prints table of contents page
\newcommand\tocpage{\section*{\toctitle}
\begin{multicols*}{3}{
\tableofcontents
}\end{multicols*}}

%%  Makes TOC items left justified even when they span multiple lines
%   https://tex.stackexchange.com/questions/283730/left-align-toc-items-when-using-tableofcontents 
\makeatletter
\bgroup
\advance\@flushglue by \@tocrmarg
\xdef\@tocrmarg{\the\@flushglue}%
\egroup
\makeatother

%% No hyphenation
\hyphenpenalty=10000

%%  Set spacing between multicols columns
\setlength\columnsep{1cm}

\pagestyle{empty}

\begin{document}
\raggedright

\tocpage

)";
}