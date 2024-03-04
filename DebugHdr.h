// Traces table
struct tr {
    char name;
    int value;
};
struct tr Traces[20];
int nTr = 0;
// Debugger functions
void add_TT(char var) {
    Traces[nTr++].name = var;
}
void display_TT(char var) {
    for(int i = 0; i < nTr; i++){
        if(Traces[i].name != var) continue;
        printf("%c: %d\n", var, Traces[i].value);
        getchar();
    }
}
void update_TT(char var, int val) {
    for(int i = 0; i < nTr; i++){
        if(Traces[i].name != var) continue;
        Traces[i].value = val;
    }
}
