#include "catch.hpp"
#include "data/parameters.hpp"
#include "ortc.hpp"

using namespace mediasoupclient;

TEST_CASE("GetExtendedCapabilities", "[ortc][GetExtendedCapabilities]")
{
	SECTION("succeeds if localCaps equals remoteCaps")
	{
		json remoteCaps = generateRouterRtpCapabilities();
		json localCaps  = generateRouterRtpCapabilities();

		auto extendedRtpCapabilities = ortc::getExtendedCapabilities(localCaps, remoteCaps);

		REQUIRE(extendedRtpCapabilities["codecs"].size() == 3);

		auto codecs = extendedRtpCapabilities["codecs"];
		REQUIRE(codecs[0]["name"] == "opus");

		REQUIRE(codecs[1]["name"] == "VP8");
		REQUIRE(codecs[1]["recvRtxPayloadType"] == 102);
		REQUIRE(codecs[1]["sendRtxPayloadType"] == 102);

		REQUIRE(codecs[2]["name"] == "H264");
		REQUIRE(codecs[2]["recvRtxPayloadType"] == 104);
		REQUIRE(codecs[2]["sendRtxPayloadType"] == 104);

		REQUIRE(extendedRtpCapabilities["headerExtensions"].size() == 8);
	}

	SECTION("succeeds if localCaps is a subset of remoteCaps")
	{
		json remoteCaps = generateRouterRtpCapabilities();
		json localCaps  = generateRouterRtpCapabilities();

		// Remove the second codec "VP8"
		auto it = localCaps["codecs"].begin();
		it++;
		localCaps["codecs"].erase(it);

		auto extendedRtpCapabilities = ortc::getExtendedCapabilities(localCaps, remoteCaps);

		REQUIRE(extendedRtpCapabilities["codecs"].size() == 2);

		auto codecs = extendedRtpCapabilities["codecs"];
		REQUIRE(codecs[0]["name"] == "opus");

		REQUIRE(codecs[1]["name"] == "H264");
		REQUIRE(codecs[1]["recvRtxPayloadType"] == 104);
		REQUIRE(codecs[1]["sendRtxPayloadType"] == 104);

		REQUIRE(extendedRtpCapabilities["headerExtensions"].size() == 8);
	}
}

TEST_CASE("GetRecvRtpCapabilities", "[GetRecvRtpCapabilities]")
{
	SECTION("succeeds if localCaps equals remoteCaps")
	{
		json remoteCaps = generateRouterRtpCapabilities();
		json localCaps  = generateRouterRtpCapabilities();

		auto extendedRtpCapabilities = ortc::getExtendedCapabilities(localCaps, remoteCaps);
		auto recvRtpCapabilities     = ortc::getRecvRtpCapabilities(extendedRtpCapabilities);

		REQUIRE(recvRtpCapabilities["codecs"].size() == 5);

		auto codecs = recvRtpCapabilities["codecs"];

		REQUIRE(codecs[0]["name"] == "opus");
		REQUIRE(codecs[1]["name"] == "VP8");
		REQUIRE(codecs[2]["name"] == "rtx");
		REQUIRE(codecs[3]["name"] == "H264");
		REQUIRE(codecs[4]["name"] == "rtx");
	}

	SECTION("succeeds if localCaps is a subset of remoteCaps")
	{
		json remoteCaps = generateRouterRtpCapabilities();
		json localCaps  = generateRouterRtpCapabilities();

		// Remove the second codec "VP8"
		auto it = localCaps["codecs"].begin();
		it++;
		localCaps["codecs"].erase(it);

		auto extendedRtpCapabilities = ortc::getExtendedCapabilities(localCaps, remoteCaps);
		auto recvRtpCapabilities     = ortc::getRecvRtpCapabilities(extendedRtpCapabilities);

		REQUIRE(recvRtpCapabilities["codecs"].size() == 3);

		auto codecs = recvRtpCapabilities["codecs"];

		REQUIRE(codecs[0]["name"] == "opus");
		REQUIRE(codecs[1]["name"] == "H264");
		REQUIRE(codecs[2]["name"] == "rtx");
	}
}

TEST_CASE("GetSendingRtpParameters", "[GetSendingRtpParameters]")
{
	SECTION("succeeds if localCaps equals remoteCaps")
	{
		json remoteCaps = generateRouterRtpCapabilities();
		json localCaps  = generateRouterRtpCapabilities();

		auto extendedRtpCapabilities = ortc::getExtendedCapabilities(localCaps, remoteCaps);
		auto audioSendRtpParameters  = ortc::getSendingRtpParameters("audio", extendedRtpCapabilities);

		REQUIRE(audioSendRtpParameters["codecs"].size() == 1);
		REQUIRE(audioSendRtpParameters["codecs"][0]["name"] == "opus");

		auto videoSendRtpParameters = ortc::getSendingRtpParameters("video", extendedRtpCapabilities);

		REQUIRE(videoSendRtpParameters["codecs"].size() == 2);
		REQUIRE(videoSendRtpParameters["codecs"][0]["name"] == "VP8");
		REQUIRE(videoSendRtpParameters["codecs"][1]["name"] == "rtx");
	}
}

TEST_CASE("ortc::canSend", "[ortc::canSend]")
{
	json remoteCaps = generateRouterRtpCapabilities();
	json localCaps  = generateRouterRtpCapabilities();

	auto extendedRtpCapabilities = ortc::getExtendedCapabilities(localCaps, remoteCaps);

	SECTION("it can send audio and video if audio and video codecs are present")
	{
		REQUIRE(ortc::canSend("audio", extendedRtpCapabilities));
		REQUIRE(ortc::canSend("video", extendedRtpCapabilities));
	}

	SECTION("it cannot send audio if no audio codec is present")
	{
		// Remove the first codec "opus"
		auto it = extendedRtpCapabilities["codecs"].begin();
		extendedRtpCapabilities["codecs"].erase(it);

		REQUIRE(!ortc::canSend("audio", extendedRtpCapabilities));
		REQUIRE(ortc::canSend("video", extendedRtpCapabilities));
	}

	SECTION("it cannot send audio nor video if no codec is present")
	{
		extendedRtpCapabilities["codecs"] = json::array();

		REQUIRE(!ortc::canSend("audio", extendedRtpCapabilities));
		REQUIRE(!ortc::canSend("video", extendedRtpCapabilities));
	}
}

TEST_CASE("ortc::canReceive", "[ortc::canReceive]")
{
	json remoteCaps = generateRouterRtpCapabilities();
	json localCaps  = generateRouterRtpCapabilities();

	auto extendedRtpCapabilities = ortc::getExtendedCapabilities(localCaps, remoteCaps);

	SECTION("it can receive")
	{
		auto rtpParameters = R"(
		{
			"codecs" :
			[
				{
					"name"         : "opus",
					"mimeType"     : "audio/opus",
					"kind"         : "audio",
					"clockRate"    : 48000,
					"payloadType"  : 100,
					"channels"     : 2,
					"rtcpFeedback" : [],
					"parameters"   :
					{
						"useinbandfec" : "1"
					}
				}
			]
		})"_json;

		REQUIRE(ortc::canReceive(rtpParameters, extendedRtpCapabilities));
	}

	SECTION("it cannot receive if empty rtpParameters")
	{
		auto rtpParameters = R"(
		{
			"codecs" : []
		})"_json;

		REQUIRE(!ortc::canReceive(rtpParameters, extendedRtpCapabilities));
	}

	SECTION("it cannot receive if no matching payload type")
	{
		auto rtpParameters = R"(
		{
			"codecs" :
			[
				{
					"name"         : "opus",
					"mimeType"     : "audio/opus",
					"kind"         : "audio",
					"clockRate"    : 48000,
					"payloadType"  : 96,
					"channels"     : 2,
					"rtcpFeedback" : [],
					"parameters"   :
					{
						"useinbandfec" : "1"
					}
				}
			]
		})"_json;

		REQUIRE(!ortc::canReceive(rtpParameters, extendedRtpCapabilities));
	}
}
