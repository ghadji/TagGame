#pragma once

#include "Walnut/Layer.h"

#include "HeadlessConsole.h"

#include "Walnut/Networking/Server.h"

#include <glm/glm.hpp>
#include <map>

using namespace Walnut;

namespace TagGame
{
	class ServerLayer : public Layer
	{
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(float ts) override;
		virtual void OnUIRender() override;
	private:
		void OnConsoleMessage(std::string_view message); 

		void OnClientConnected(const ClientInfo& clientInfo);
		void OnClientDisconnected(const ClientInfo& clientInfo);
		void OnDataReceived(const ClientInfo& clientInfo, const Buffer buffer);
	private:
		HeadlessConsole m_Console;
		Server m_Server{ 8192 };

		struct PlayerData
		{
			glm::vec2 Position;
			glm::vec2 Velocity;
		};

		std::mutex m_PlayerDataMutex;
		std::map<uint32_t, PlayerData> m_PlayerData;
	};
}