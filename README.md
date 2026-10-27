# SLAM – LiDAR odometry e ricostruzione 3D

Applicazione C++17 che ricostruisce una mappa 3D a partire da una sequenza di
scansioni LiDAR (file `.pcd`). Ogni nuova scansione viene allineata alla
precedente con un algoritmo **ICP** (Iterative Closest Point) scritto da zero;
la posa stimata del sensore serve ad accumulare i punti in una **mappa globale**,
visualizzata in tempo reale con OpenGL.

## Caratteristiche

- **Odometria con ICP point-to-point**: corrispondenze tramite nearest neighbor,
  rifiuto degli outlier a soglia di distanza e stima di rotazione e traslazione
  con SVD (Eigen).
- **KD-tree** 3D implementato in proprio per accelerare la ricerca dei vicini.
- **Caricamento asincrono** dei frame: il parsing dei file PCD binari avviene in
  un thread separato, senza bloccare il rendering.
- **Visualizzatore OpenGL 3.3** con griglia di riferimento, mappa a nuvola di
  punti e camera libera in stile FPS.

## Struttura del progetto

```
include/   header (Camera, KDTree, Odometry, Scene, Shader, Utils, Visualizer)
src/       implementazioni e main
shaders/   vertex e fragment shader GLSL
```

| Modulo       | Ruolo                                                         |
|--------------|---------------------------------------------------------------|
| `Odometry`   | ICP: stima della trasformazione rigida fra due scansioni      |
| `KDTree`     | Struttura spaziale per la ricerca del punto più vicino        |
| `Scene`      | Caricamento asincrono dei frame                               |
| `Utils`      | Parser PCD binario, input da tastiera                         |
| `Visualizer` | Rendering di griglia e mappa globale                          |
| `Camera`     | Camera libera controllata da tastiera e mouse                 |

## Dipendenze

| Libreria | Uso                          |
|----------|------------------------------|
| GLFW     | finestra e input             |
| GLAD     | caricamento delle funzioni OpenGL |
| GLM      | matematica per la grafica    |
| Eigen    | algebra lineare (SVD)        |

Le dipendenze si gestiscono con [vcpkg](https://github.com/microsoft/vcpkg).

## Compilazione

Prerequisiti: CMake, un compilatore C++17 (ad esempio Visual Studio 2022 su
Windows) e vcpkg.

```bash
vcpkg install glfw3 glm glad eigen3
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=<percorso-vcpkg>/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
```

Gli shader vengono copiati nella cartella di build durante la configurazione.
Esegui il programma dalla cartella `build`, in modo che trovi `shaders/`.

## Dataset

Il programma legge una cartella di file `.pcd` **binari** (campi XYZ in float
a 32 bit) ordinati per nome. È stato provato con il dataset Anovox.

Il percorso della cartella è per ora fisso in `src/main.cpp` (variabile
`pcdPath`): modificalo con la posizione del tuo dataset prima di compilare.

## Controlli

| Tasto         | Azione                                   |
|---------------|------------------------------------------|
| `W` `A` `S` `D` | muove la camera                        |
| `E` / `Q`     | sale / scende                            |
| `TAB`         | attiva o disattiva il controllo col mouse |
| `SPAZIO`      | pausa / riprendi la ricostruzione        |
| `ESC`         | esce                                     |

Al termine della sequenza la ricostruzione si mette in pausa e puoi esplorare
liberamente la mappa finale.

## Come funziona

1. Si carica il frame *t* e lo si confronta con il frame *t−1*.
2. Per ogni punto sottocampionato si cerca il vicino più prossimo nel KD-tree;
   le coppie troppo distanti vengono scartate.
3. Si calcolano i baricentri e la matrice di covarianza incrociata; la sua SVD
   fornisce la rotazione ottima, da cui si ricava la traslazione.
4. Il passo viene applicato e si ripete per un numero fisso di iterazioni.
5. La trasformazione relativa aggiorna la posa del sensore, che porta i punti
   del frame corrente nel sistema di riferimento globale.

## Limiti e possibili sviluppi

- Il sottocampionamento è fisso e non c'è un criterio di convergenza per ICP.
- Nessuna chiusura di loop né ottimizzazione globale (pose graph): la deriva si
  accumula lungo la traiettoria.
- Percorso del dataset da rendere configurabile (argomento da riga di comando).
- Supporto a PCD con campi aggiuntivi (intensità, colore) e formato ASCII.

## Licenza

Da definire.
