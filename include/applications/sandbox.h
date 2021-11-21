#pragma once
#include "application.h"


class Sandbox : public Application
{
public:
    Sandbox();
protected:
    void CreateScene() override;
};