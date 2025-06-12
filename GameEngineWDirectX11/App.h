#pragma once
#include "EngineBase.h"
#include "Model.h"
#include "CubeMapping.h"

namespace Luna {
	class App : public EngineBase {
      public:
        App();

		virtual bool Initialize() override;
        virtual void UpdateGUI() override;
        virtual void Update(float dt) override;
        virtual void Render() override;
        void UpdateLight();

      protected:
        // Don't use shared_ptr.
        Model m_groundModel;
        CubeMapping m_cubeMapping;
        Light m_pointLight;
        Vector3 m_lightPosition = Vector3(0.0f, 1.0f, 0.0f);
        bool m_usePerspectiveProjection = true;
	};
    }
