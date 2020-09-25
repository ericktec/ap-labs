

int infof(const char *format, ...);
int warnf(const char *format, ...);
int errorf(const char *format, ...);
int panicf(const char *format, ...);


int main() {
    infof("Esto es un log infof %s", "Esta es la variable\n");
    warnf("Esto es un log warnf %s", "Esta es la variable\n");
    errorf("Esto es un log errorf %s", "Esta es la variable\n");
    panicf("Esto es un log panicf %s", "Esta es la variable\n");
    return 0;
}
