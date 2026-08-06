#include "common/macros.h"
#include "gameroot.h"
#include "context/context.h"
#include "Comm/ITableGame.h"
#include "utils/tarslog.h"
#include "message/sendclientmessage.h"
#include "message/sendroommessage.h"

namespace game
{
    namespace logic
    {
        namespace roomlogic
        {
            void SetWinCard(void const *p, GameRoot *root)
            {
                PERFSTATS_ENTRY();
                __TRY__

                using namespace RoomSo;
                using namespace context;
                using namespace message;

                TGAME_SetWinCard const *nnrs = static_cast<TGAME_SetWinCard const *>(p);
                DLOG_TRACE("roomid:"<<root->roomid()<<", "<<"TGAME_SetWinCard_E uid:" << nnrs->uid <<", winNum:"<< nnrs->winNum <<", preWinNum:"<<nnrs->preWinNum);

                root->con->setTotalProfit(nnrs->winNum);

                std::vector<cid_t> vecRoboter;
                std::vector<cid_t> vecplayer;

                std::map<cid_t, User> &usermap = root->con->refUserMap();
                for (auto it = usermap.begin(); it != usermap.end(); it++)
                {
                    if(it->second.isMidSit())
                    {
                        continue;
                    }
                    if(it->second.isRobot())
                    {
                        vecRoboter.push_back(it->first);
                    }
                    else
                    {
                        vecplayer.push_back(it->first);
                    }
                }

                if(vecRoboter.size() == 0 || vecplayer.size() == 0)
                {
                    root->con->setWinnerCid(nil_cid);
                    return;
                }
                for (auto it = usermap.begin(); it != usermap.end(); it++)
                {
                    if(it->second.isMidSit())
                    {
                        continue;
                    }
                    if((nnrs->player_type == 1 && !it->second.isRobot()) || (nnrs->player_type == 2 && it->second.isRobot()))
                    {
                        root->con->setWinnerCid(it->first);
                        break;
                    }
                }

                __CATCH__
                PERFSTATS_EXIT();
            }
        }
    }
}
