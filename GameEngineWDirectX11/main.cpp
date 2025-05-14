#include "pch.h"
#include <iostream>
#include "App.h"

using namespace std;
int main() {
    Luna::App app;

    if (!app.Initialize()) {
        return -1;
    }

    return app.Run();
}
