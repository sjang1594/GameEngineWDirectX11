#pragma once
#include <algorithm>
#include <directxtk/SimpleMath.h>
#include <iostream>
#include <memory>
#include <vector>
#include <string>

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

      protected:
	};
    }
