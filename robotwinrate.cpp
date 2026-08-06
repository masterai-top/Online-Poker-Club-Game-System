#include "common/macros.h"
#include "gameroot.h"
#include "context/context.h"
#include "Comm/ITableGame.h"
#include "utils/tarslog.h"
#include "process/process.h"

namespace game
{
    namespace logic
    {
        namespace roomlogic
        {
            void SetRobotWinRate(void const *p, GameRoot *root)
            {
                PERFSTATS_ENTRY();
                __TRY__

                using namespace RoomSo;
                using namespace context;

                TRobotDecideRsp const *nnrs = static_cast<TRobotDecideRsp const *>(p);
                //DLOG_TRACE("roomid:"<<root->roomid()<<", "<<"TGAME_RobotWRate_E.");

                if(!nnrs)
                {
                    //DLOG_TRACE("roomid:"<<root->roomid()<<", "<<"TGAME_RobotWRate_E nnrs is nullptr.");
                    return;
                }

                for(auto it : nnrs->robotRate)
                {
                    User *user = root->con->getUserByUid(it.first);
                    if(!user)
                    {
                        continue;
                    }
                    user->setRobotUserParam("wRate", "", (it.second / 100.00));
                    DLOG_TRACE("roomid:"<<root->roomid()<<", "<<"uid:"<< it.first<<", wrate:"<< it.second);
                }

                __CATCH__
                PERFSTATS_EXIT();
            }
        }
    }
}
