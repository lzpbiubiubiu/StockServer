#include "GlobalData.h"

namespace Core
{
    GlobalData::GlobalData()
    {
    }

    GlobalData::~GlobalData()
    {
    }

    CommonConfig& GlobalData::GetConfig()
    {
        return m_config;
    }
}