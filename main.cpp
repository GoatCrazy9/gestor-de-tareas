#include <iostream>
#include <string>
using namespace std;

// limites para no pasarnos del arreglo
const int MAX_TAREAS = 100;
const int MAX_ACCIONES = 200;

// ESTRUCTURAS 

// datos de una tarea
struct tarea{
    int id;
    char descripcion[80];
    int prioridad; // 1 baja, 2 media, 3 urgente
    int estado;    // 0 pendiente, 1 completada
};

// dato que se guarda en la pila para deshacer
struct accion{
    int tipo;      // 1 crear, 2 eliminar, 3 modificar, 4 completar
    tarea t;       // copia de la tarea antes del cambio
    int indice;    // posicion referencial en el arreglo
};

// nodo para la lista enlazada de urgentes
struct nodoU{
    tarea t;
    nodoU *sgte;
};

typedef nodoU* TUrg;

// ARREGLO DE TAREAS 

// aqui se guardan todas las tareas
tarea V[MAX_TAREAS];
int nT = 0; // cantidad actual de tareas

// PILA HISTORIAL

// pila para guardar acciones y poder deshacer
accion P[MAX_ACCIONES];
int tope = -1;

// COLA FIFO 

// cola circular para ids de tareas
int C[MAX_TAREAS];
int frente = -1, fin = -1;

// LISTA ENLAZADA URGENTES 

// cabeza de la lista de urgentes
TUrg urg = NULL;

// UTILITARIOS

// busca una tarea por id en el arreglo
// devuelve la posicion o -1 si no esta
int buscarIndice(int id){
    for(int i=0; i<nT; i++){
        if(V[i].id == id) return i;
    }
    return -1;
}

// imprime una tarea bonita en una sola linea
void mostrarTarea(tarea t){
    cout << "ID: " << t.id
         << " | Desc: " << t.descripcion
         << " | Pri: " << t.prioridad
         << " | Estado: " << (t.estado==0 ? "pendiente" : "completada")
         << endl;
}

// PILA

// guarda una accion en la pila
void apilarAccion(accion a){
    if(tope == MAX_ACCIONES-1){
        cout << "pila llena, no se registra accion" << endl;
        return;
    }
    tope++;
    P[tope] = a;
}

// saca la ultima accion de la pila
// devuelve 1 si pudo, 0 si estaba vacia
int desapilarAccion(accion &a){
    if(tope == -1) return 0;
    a = P[tope];
    tope--;
    return 1;
}

// COLA

// revisa si la cola esta vacia
int colaVacia(){
    return (frente == -1);
}

// revisa si la cola esta llena
int colaLlena(){
    return ( (fin==MAX_TAREAS-1 && frente==0) || (fin+1==frente) );
}

// mete un id a la cola
void encolar(int id){
    if(colaLlena()){
        cout << "cola llena" << endl;
        return;
    }
    if(frente == -1) frente = 0;
    fin = (fin + 1) % MAX_TAREAS;
    C[fin] = id;
}

// saca el primer id de la cola
int desencolar(int &id){
    if(colaVacia()) return 0;

    id = C[frente];
    if(frente == fin){
        frente = -1;
        fin = -1;
    }else{
        frente = (frente + 1) % MAX_TAREAS;
    }
    return 1;
}

// muestra los ids que estan en cola
void mostrarCola(){
    if(colaVacia()){
        cout << "cola vacia" << endl;
        return;
    }
    cout << "cola ids: ";
    int i = frente;
    while(true){
        cout << C[i] << " ";
        if(i == fin) break;
        i = (i + 1) % MAX_TAREAS;
    }
    cout << endl;
}

// LISTA URGENTES

// borra la lista completa para volver a armarla
void limpiarUrgentes(){
    while(urg != NULL){
        TUrg aux = urg;
        urg = urg->sgte;
        delete aux;
    }
}

// inserta una tarea urgente en la lista
// la dejo ordenada por id para que se vea mas claro al mostrar
void insertarUrgenteOrdenado(tarea t){
    TUrg temp = new nodoU;
    temp->t = t;
    temp->sgte = NULL;

    if(urg == NULL){
        urg = temp;
        return;
    }

    if(t.id < urg->t.id){
        temp->sgte = urg;
        urg = temp;
        return;
    }

    TUrg aux = urg;
    while(aux->sgte != NULL && aux->sgte->t.id < t.id){
        aux = aux->sgte;
    }
    temp->sgte = aux->sgte;
    aux->sgte = temp;
}

// arma de nuevo la lista con las urgentes pendientes
void refrescarUrgentes(){
    limpiarUrgentes();
    for(int i=0; i<nT; i++){
        if(V[i].prioridad == 3 && V[i].estado == 0){
            insertarUrgenteOrdenado(V[i]);
        }
    }
}

// muestra solo las urgentes pendientes
void mostrarUrgentes(){
    if(urg == NULL){
        cout << "no hay urgentes pendientes" << endl;
        return;
    }
    cout << "lista de urgentes:" << endl;
    TUrg aux = urg;
    while(aux != NULL){
        mostrarTarea(aux->t);
        aux = aux->sgte;
    }
}

// MATRIZ REPORTE

// cuenta tareas por estado y prioridad y lo muestra
void mostrarReporte(){
    int R[2][3] = { {0,0,0}, {0,0,0} }; // estado por prioridad

    for(int i=0; i<nT; i++){
        int e = V[i].estado;
        int p = V[i].prioridad - 1;
        if(e>=0 && e<=1 && p>=0 && p<=2) R[e][p]++;
    }

    cout << "\nreporte matriz estado vs prioridad" << endl;
    cout << "estado\\pri | baja 1  media 2  urgente 3" << endl;
    cout << "pendiente  |   " << R[0][0] << "       " << R[0][1] << "        " << R[0][2] << endl;
    cout << "completada |   " << R[1][0] << "       " << R[1][1] << "        " << R[1][2] << endl;
}

// OPERACIONES

// crea tarea y la mete al arreglo
// tambien registra en pila, cola y lista de urgentes
void crearTarea(){
    if(nT == MAX_TAREAS){
        cout << "vector lleno, no se puede crear" << endl;
        return;
    }

    tarea t;
    cout << "ingrese id: ";
    cin >> t.id;

    if(buscarIndice(t.id) != -1){
        cout << "id repetido" << endl;
        return;
    }

    cin.ignore();
    cout << "ingrese descripcion: ";
    cin.getline(t.descripcion, 80);

    cout << "prioridad 1 baja 2 media 3 urgente: ";
    cin >> t.prioridad;
    if(t.prioridad < 1 || t.prioridad > 3) t.prioridad = 1;

    t.estado = 0;

    V[nT] = t;

    // guardo accion para deshacer
    accion a;
    a.tipo = 1;
    a.t = t;
    a.indice = nT;
    apilarAccion(a);

    nT++;

    // se encola el id para procesarlo luego
    encolar(t.id);

    // se actualiza la lista de urgentes
    refrescarUrgentes();

    cout << "tarea creada" << endl;
}

// muestra todas las tareas del arreglo
void mostrarTareas(){
    if(nT == 0){
        cout << "no hay tareas" << endl;
        return;
    }
    cout << "\nlista de tareas:" << endl;
    for(int i=0; i<nT; i++){
        mostrarTarea(V[i]);
    }
}

// cambia descripcion y prioridad
void modificarTarea(){
    int id;
    cout << "id a modificar: ";
    cin >> id;

    int idx = buscarIndice(id);
    if(idx == -1){
        cout << "no existe id" << endl;
        return;
    }

    // guardo el estado antes del cambio
    accion a;
    a.tipo = 3;
    a.t = V[idx];
    a.indice = idx;
    apilarAccion(a);

    cin.ignore();
    cout << "nueva descripcion: ";
    cin.getline(V[idx].descripcion, 80);

    cout << "nueva prioridad 1 baja 2 media 3 urgente: ";
    cin >> V[idx].prioridad;
    if(V[idx].prioridad < 1 || V[idx].prioridad > 3) V[idx].prioridad = 1;

    refrescarUrgentes();
    cout << "tarea modificada" << endl;
}

// marca tarea como completada
void completarTarea(){
    int id;
    cout << "id a completar: ";
    cin >> id;

    int idx = buscarIndice(id);
    if(idx == -1){
        cout << "no existe id" << endl;
        return;
    }

    // guardo el estado antes de completar
    accion a;
    a.tipo = 4;
    a.t = V[idx];
    a.indice = idx;
    apilarAccion(a);

    V[idx].estado = 1;
    refrescarUrgentes();
    cout << "tarea completada" << endl;
}

// elimina una tarea y corre el arreglo para no dejar huecos
void eliminarTarea(){
    int id;
    cout << "id a eliminar: ";
    cin >> id;

    int idx = buscarIndice(id);
    if(idx == -1){
        cout << "no existe id" << endl;
        return;
    }

    // guardo la tarea antes de borrar
    accion a;
    a.tipo = 2;
    a.t = V[idx];
    a.indice = idx;
    apilarAccion(a);

    // corrimiento a la izquierda
    for(int i=idx; i<nT-1; i++){
        V[i] = V[i+1];
    }
    nT--;

    refrescarUrgentes();
    cout << "tarea eliminada" << endl;
}

// revierte la ultima accion guardada en la pila
void deshacer(){
    accion a;
    if(!desapilarAccion(a)){
        cout << "no hay acciones" << endl;
        return;
    }

    // si fue crear, se borra esa tarea
    if(a.tipo == 1){
        int idx = buscarIndice(a.t.id);
        if(idx != -1){
            for(int i=idx; i<nT-1; i++) V[i] = V[i+1];
            nT--;
            cout << "deshacer: se elimino la tarea creada" << endl;
        }else{
            cout << "deshacer: no se encontro la tarea" << endl;
        }
    }
    // si fue eliminar, se inserta de nuevo en su posicion
    else if(a.tipo == 2){
        if(nT == MAX_TAREAS){
            cout << "no hay espacio para restaurar" << endl;
            return;
        }

        int pos = a.indice;
        if(pos < 0) pos = 0;
        if(pos > nT) pos = nT;

        for(int i=nT; i>pos; i--){
            V[i] = V[i-1];
        }
        V[pos] = a.t;
        nT++;

        cout << "deshacer: se restauro la tarea eliminada" << endl;
    }
    // si fue modificar o completar, se restaura la copia anterior
    else if(a.tipo == 3 || a.tipo == 4){
        int idx = buscarIndice(a.t.id);
        if(idx != -1){
            V[idx] = a.t;
            cout << "deshacer: se restauro la tarea" << endl;
        }else{
            cout << "deshacer: no se encontro la tarea" << endl;
        }
    }

    refrescarUrgentes();
}

// procesa la cola en orden fifo
void procesarCola(){
    int id;
    if(!desencolar(id)){
        cout << "cola vacia" << endl;
        return;
    }

    cout << "procesando id: " << id << endl;

    int idx = buscarIndice(id);
    if(idx == -1){
        cout << "la tarea ya no existe, se omite" << endl;
        return;
    }

    mostrarTarea(V[idx]);

    int op;
    cout << "marcar como completada 1 si 0 no: ";
    cin >> op;

    if(op == 1){
        // guardo para deshacer
        accion a;
        a.tipo = 4;
        a.t = V[idx];
        a.indice = idx;
        apilarAccion(a);

        V[idx].estado = 1;
        refrescarUrgentes();
        cout << "completada en procesamiento" << endl;
    }
}

// MENU

void menu(){
    cout << "\nmenu gestor de tareas" << endl;
    cout << "1. crear tarea" << endl;
    cout << "2. mostrar tareas" << endl;
    cout << "3. modificar tarea" << endl;
    cout << "4. completar tarea" << endl;
    cout << "5. eliminar tarea" << endl;
    cout << "6. ver urgentes lista" << endl;
    cout << "7. procesar cola fifo" << endl;
    cout << "8. deshacer pila" << endl;
    cout << "9. reporte matriz" << endl;
    cout << "10. salir" << endl;
    cout << "opcion: ";
}

int main(){
    int op;

    // ciclo principal del programa
    do{
        menu();
        cin >> op;

        switch(op){
            case 1: crearTarea(); break;
            case 2: mostrarTareas(); break;
            case 3: modificarTarea(); break;
            case 4: completarTarea(); break;
            case 5: eliminarTarea(); break;
            case 6: mostrarUrgentes(); break;
            case 7: procesarCola(); break;
            case 8: deshacer(); break;
            case 9: mostrarReporte(); break;
            case 10: cout << "saliendo..." << endl; break;
            default: cout << "opcion invalida" << endl;
        }

        // esto es solo para ver como va la cola mientras pruebas
        if(op >= 1 && op <= 9){
            cout << endl;
            mostrarCola();
        }

    }while(op != 10);

    // al final libero memoria de la lista
    limpiarUrgentes();
    return 0;
}
