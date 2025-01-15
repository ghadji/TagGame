#pragma once

#include "Walnut//Application.h"
#include "Walnut/Layer.h"

#include "Walnut/Networking/Client.h"

#include <glm/glm.hpp>

using namespace Walnut;

namespace TagGame {
	class ClientLayer : public Layer
	{
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(float ts) override;
		virtual void OnUIRender() override;

	private:
		void OnDataReceived(const Buffer buffer);

	private:
		glm::vec2 m_PlayerPosition{ 50, 50 };
		glm::vec2 m_PlayerVelocity{ 0, 0 };

		std::string m_ServerAdddress;

		Walnut::Client m_Client;
		uint32_t m_PlayerID = 0;

		struct PlayerData
		{
			glm::vec2 Position;
			glm::vec2 Velocity;
		};

		std::mutex m_PlayerDataMutex;
		std::map<uint32_t, PlayerData> m_PlayerData;
	};
}