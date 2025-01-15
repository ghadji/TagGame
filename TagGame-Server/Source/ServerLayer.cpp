#include "ServerLayer.h"

#include "Walnut/Serialization/BufferStream.h"
#include "Walnut/Core/Log.h"

#include <chrono>
#include <ServerPacket.h>

namespace TagGame
{

	static Buffer s_ScratchBuffer;

	void ServerLayer::OnAttach()
	{
		s_ScratchBuffer.Allocate(10 * 1024 * 1024);

		m_Console.SetMessageSendCallback([this](std::string_view message) { OnConsoleMessage(message); });

		m_Server.SetClientConnectedCallback([this](const ClientInfo clientInfo) { OnClientConnected(clientInfo); });
		m_Server.SetClientDisconnectedCallback([this](const ClientInfo clientInfo) { OnClientDisconnected(clientInfo); });
		m_Server.SetDataReceivedCallback([this](const ClientInfo clientInfo, const Buffer buffer) { OnDataReceived(clientInfo, buffer); });

		m_Server.Start();
	}

	void ServerLayer::OnDetach()
	{
		m_Server.Stop(); 
	}

	void ServerLayer::OnUpdate(float ts)
	{
		BufferStreamWriter stream(s_ScratchBuffer);
		stream.WriteRaw(PacketType::ClientUpdate);
		m_PlayerDataMutex.lock();
		{
			stream.WriteMap(m_PlayerData);
		}
		m_PlayerDataMutex.unlock();

		m_Server.SendBufferToAllClients(stream.GetBuffer());
		 
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(5ms);
	}

	void ServerLayer::OnUIRender()
	{

	}

	void ServerLayer::OnConsoleMessage(std::string_view message)
	{
		if (message.starts_with('/'))
		{
			// command
			std::cout << "You called the " << message << " command!\n";
		}
	}

	void ServerLayer::OnClientConnected(const ClientInfo& clientInfo)
	{
		WL_INFO_TAG("Server", "Client connected! ID={}", clientInfo.ID);

		// BufferStreamWriter points to a portion of it so that we dont have to reallocate memory every time we make a new buffer/stream
		BufferStreamWriter stream(s_ScratchBuffer);
		stream.WriteRaw(PacketType::ClientConnect);
		stream.WriteRaw(clientInfo.ID);
		m_Server.SendBufferToClient(clientInfo.ID, stream.GetBuffer());
	}

	void ServerLayer::OnClientDisconnected(const ClientInfo& clientInfo)
	{
		WL_INFO_TAG("Server", "Client disconnected! ID={}", clientInfo.ID);
	}

	void ServerLayer::OnDataReceived(const ClientInfo& clientInfo, const Buffer buffer)
	{
		BufferStreamReader stream(buffer);
		PacketType type;
		stream.ReadRaw(type);
		switch (type)
		{
			case PacketType::ClientUpdate:

				m_PlayerDataMutex.lock();
				{
					PlayerData& playerData = m_PlayerData[clientInfo.ID];
					stream.ReadRaw<glm::vec2>(playerData.Position);
					stream.ReadRaw<glm::vec2>(playerData.Velocity);
				}
				m_PlayerDataMutex.unlock();

				break;
		}
	}
}