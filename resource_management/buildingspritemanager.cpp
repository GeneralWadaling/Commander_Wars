#include "resource_management/buildingspritemanager.h"

BuildingSpriteManager::BuildingSpriteManager()
    : RessourceManagement<BuildingSpriteManager>("/images/building/res.xml",
                                                  "/scripts/building")
{
    Interpreter::setCppOwnerShip(this);
#ifdef GRAPHICSUPPORT
    setObjectName("BuildingSpriteManager");
#endif
}

void BuildingSpriteManager::removeRessource(QString id)
{
    for (qint32 i = 0; i < m_loadedRessources.size(); ++i)
    {
        if (m_loadedRessources[i] == id)
        {
            m_loadedRessources.removeAt(i);
            break;
        }
    }
}
