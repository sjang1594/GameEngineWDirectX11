#pragma once
#include <algorithm>
#include <directxtk/SimpleMath.h>
#include <memory>

#include "EngineBase.h"

namespace Luna {
	using DirectX::SimpleMath::Matrix;
	using DirectX::SimpleMath::Vector3;
	class App : public EngineBase {
      public:
        App();

		virtual bool Initialize() override;
        virtual void UpdateGUI() override;
        virtual void Update(float dt) override;
        virtual void Render() override;
        void SetupSkyBox(const float &scale);
        void InitializeObjects();

        bool _usePerspectiveProjection = true;
        bool m_drawAsWire = false;

      protected:
        std::shared_ptr<class Model> m_skybox;
        std::shared_ptr<class Model> m_ground;
        std::vector<std::shared_ptr<class Model>> m_ojbectList; 
	};
    }
