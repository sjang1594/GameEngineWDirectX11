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

      protected:
        // Don't use shared_ptr.
        Model m_groundModel;
        CubeMapping m_cubeMapping;
        bool m_usePerspectiveProjection = true;
	};
    }
