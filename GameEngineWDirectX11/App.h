#pragma once
#include "EngineBase.h"
#include "Model.h"

namespace Luna {
using DirectX::BoundingSphere;
using DirectX::BoundingOrientedBox;

class App : public EngineBase {
  public:
    App();
    virtual bool Initialize() override;
    virtual void UpdateGUI() override;
    virtual void Update(float dt) override;
    virtual void Render() override;

  protected:
    // Default Objects
    shared_ptr<Model> m_skybox;
    shared_ptr<Model> m_lightSphere;
    shared_ptr<Model> m_groundModel;
    shared_ptr<Model> m_mainCharacter;

    // Object
    shared_ptr<Model> m_darkSoul;
    shared_ptr<Model> m_valak;

    BoundingSphere m_mainBoundingSphere;
    BoundingOrientedBox m_mainOrientedBoundingBox;

    Light m_light;
    bool m_usePerspectiveProjection = true;
    std::vector<shared_ptr<Model>> modelLists;
};
}
