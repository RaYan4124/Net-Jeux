import createModule from './game.js';
let Module;
let g;

var cv = document.getElementById('game_canvas');
var ctx = cv.getContext('2d');
if (!ctx) {
    console.error("Canvas non trouvé");
}

const images = {
    endpoint: new Image(),
    segment: new Image(),
    corner: new Image(),
    tee: new Image(),
    cross: new Image()
}

images.endpoint.src = '../res/ENDPOINT.png';
images.segment.src = '../res/SEGMENT.png';
images.corner.src = '../res/CORNER.png';
images.tee.src = '../res/TEE.png';
images.cross.src = '../res/CROSS.png';

function getImage(shape) {
    switch (shape) {
        case ENDPOINT: return images.endpoint;
        case SEGMENT: return images.segment;
        case CORNER: return images.corner;
        case TEE: return images.tee;
        case CROSS: return images.cross;
        default: return null; // empty
    }
}

const buttons = {
    newgame: document.getElementById("new_game_button"),
    restart: document.getElementById("restart_button"),
    undo: document.getElementById("undo_button"),
    redo: document.getElementById("redo_button"),
    solve: document.getElementById("solve_button")
}

async function main() {
    Module = await createModule({
        locateFile: (filename) => {
            if (filename.endsWith('.wasm')) {
                return './game.wasm'; // Indiquer le chemin correct
            }
            return filename;
        }
    });

    console.log("Module WebAssembly charge");
    start();
}

main();

const EMPTY = 0;      // empty shape
const ENDPOINT = 1;   // endpoint shape
const SEGMENT = 2;    // segment shape
const CORNER = 3;     // corner shape
const TEE = 4;        // tee shape
const CROSS = 5;      // cross shape
const NB_SHAPES = 6;  // nb of shapes


function printGame(g) {
    if (!ctx) return;

    //dimensions du jeux
    var nb_rows = Module._nb_rows(g);
    var nb_cols = Module._nb_cols(g);

    //taille des cases
    var cellWidth = cv.width / nb_cols;
    var cellHeight = cv.height / nb_rows;

    //Effacer le canevas avant de redessiner
    ctx.clearRect(0, 0, cv.width, cv.height);


    for (var row = 0; row < nb_rows; row++) {
        for (var col = 0; col < nb_cols; col++) {
            var s = Module._get_piece_shape(g, row, col);
            var o = Module._get_piece_orientation(g, row, col);
            //text += square2str(s, o);
            //ctx.fillStyle = "white";  //couleur de fond
            //ctx.fillRect(col * cellWidth, row * cellHeight, cellWidth, cellHeight); //positions et dimensions
            ctx.strokeStyle = "black"; //bordures
            ctx.strokeRect(col * cellWidth, row * cellHeight, cellWidth, cellHeight); //positions et dimensions

            let img = getImage(s);
            
            if(img){
                ctx.save() // Sauvegarder l'état du contexte

                //centre de la case
                var centerX = col * cellWidth + cellWidth / 2;
                var centerY = row * cellHeight + cellHeight / 2;

                ctx.translate(centerX, centerY); // déplacer le contexte au centre de la case
                ctx.rotate(o * (Math.PI / 2)); // rotation par rapport à l'orientation
                ctx.translate(-centerX, -centerY); // revenir a la position d'origine

                ctx.drawImage(img, col * cellWidth, row * cellHeight, cellWidth, cellHeight); //dessiner l'image
                ctx.restore(); // restaurer l'état du contexte
            }   
        }
    }
}

function new_game() {
    console.log("call new_game routine");
    if(g){
        Module._delete(g); 
    }
    cv.style.backgroundColor = 'rgb(235, 233, 233)';
    g = Module._new_random(5, 5, false,  Math.floor(Math.random() * 11), 0);
    Module._shuffle(g);
    play_game(g);
}

function restart_game() {
    console.log("call restart_game routine");
    if(g){
        cv.style.backgroundColor = 'rgb(235, 233, 233)';;
        Module._shuffle(g);
    }
    play_game(g);
}

function game_solve() {
    console.log("call restart_game routine");
    if(g){
        Module._solve(g);
    }
    printGame(g);
    if(Module._won(g)){
        cv.style.backgroundColor = 'rgb(45, 255, 45)';
        cv.removeEventListener("mousedown", handleClick);
    }
    
}

function handleClick(event) {
    //coordonnées du clic par rapport au canevas
    const rect = cv.getBoundingClientRect();//recuperation des coordonnees et dimensions du canevas par rapport au viewport
    const x = event.clientX - rect.left; //left : dist entre le bord gauche du canevas et et le bord gauche du viewport , clientX : coordonnee du clic par rapport au viewport
    const y = event.clientY - rect.top; //top : dist entre le bord sup du canevas et et le bord sup du viewport , clientY : coordonnee du clic par rapport au viewport

    //calculer la case cliquée
    const nb_rows = Module._nb_rows(g);
    const nb_cols = Module._nb_cols(g);
    const cellWidth = cv.width / nb_cols;
    const cellHeight = cv.height / nb_rows;

    const col = Math.floor(x / cellWidth);
    const row = Math.floor(y / cellHeight);

    console.log(`Case cliquée : ligne ${row}, colonne ${col}`);
    if(event.button == 0){ //click gauche
        Module._play_move(g, row, col, 1);
    }else if(event.button == 2){//click droit
        Module._play_move(g, row, col, -1);
    }
    
    // redessiner la grille après le mouvement
    printGame(g);
    if(Module._won(g)){
        console.log("Vous avez gagné !");
        cv.style.backgroundColor = 'green'; // changer la couleur de fond du canevas
        cv.removeEventListener("mousedown", handleClick); //desactivé le click
    }
};

function play_game(g){
    printGame(g);
    cv.addEventListener("mousedown", handleClick);
    
    window.addEventListener("beforeunload", () => {
        Module._delete(g);
    });
}

function undo_move(g){
    if(g){
        Module._undo(g);
    }
    printGame(g);
}

function redo_move(g){
    if(g){
        Module._redo(g);
    }
    printGame(g);
}

function setClickEvent(){
    cv.addEventListener("contextmenu", (e) => e.preventDefault());

    buttons.newgame.addEventListener("click", new_game);
    buttons.restart.addEventListener("click", restart_game);
    buttons.solve.addEventListener("click", game_solve);    
    buttons.undo.addEventListener("click", () => {
            undo_move(g);
        }   
    );
    buttons.redo.addEventListener("click", () => {
            redo_move(g);
        }
    );
}

function setKeyBoardEvent(){
    document.addEventListener("keydown", (event) => {
        if(event.key === "n" || event.key == "N") new_game() 
        if(event.key === "r" || event.key == "R") restart_game()
        if(event.key === "s" || event.key == "S") game_solve()
        if(event.key === "z" || event.key == "Z") undo_move(g)
        if(event.key === "y" || event.key == "Y") redo_move(g)   
    });

}

function start() {
    console.log("call start routine");
    g = Module._new_default();
    play_game(g);

    setClickEvent();
    setKeyBoardEvent();
}

