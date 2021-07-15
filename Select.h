#pragma once
#include "Main.h"

class Select : public App::Scene{
private:
    
public:
    Select(const InitData& init);
    void update() override;
    void draw() const override;
    Array<String> getDummies();
};