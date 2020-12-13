#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

const int N = 30002;
const int MaxM = 30;

const string stainfo[]={"Accepted", "Wrong_Answer", "Runtime_Error", "Time_Limit_Exceed", "ALL"};

int duration, pbCnt, teamCnt;
bool startFlag = false, fzFlag = false;
unordered_map <string, int> hashTable;

enum typeSta {AC, WA, RE, TLE, ALL};

struct Submission {
    char pbName;
    typeSta statype;
    int time;
};

struct Problem {
    int cnt, fail, firstAC;
};

struct Team {
    int rk, accnt, plty;
    string name;
    Problem pb[MaxM], fzpb[MaxM];
    Submission last[5], llast, plast[MaxM][5], pllast[MaxM];

    bool operator < (const Team& obj) const {
        if (accnt != obj.accnt) return accnt > obj.accnt;
        //if (this->calAC() != obj.calAC()) return this->calAC() > obj.calAC();
        if (plty != obj.plty) return plty < obj.plty;
        int tmp[MaxM], tmp1[MaxM];
        for (int i = 0; i < pbCnt; ++i) tmp[i] = pb[i].firstAC, tmp1[i] = obj.pb[i].firstAC;
        sort(tmp, tmp+pbCnt);
        sort(tmp1, tmp1+pbCnt);
        for (int i = pbCnt - 1; i >= 0; --i)
            if (tmp[i] != tmp1[i]) return tmp[i] < tmp1[i];
        return name < obj.name;
    }
};

Team* teamPool[N];

struct Data {
    int pos;
    bool operator < (const Data& obj) const {
        return *teamPool[pos] < *teamPool[obj.pos];
    }
};

set <Data> teamTable;
vector <Data> rkTable;

void AddTeam(const char* str) {
    if (!startFlag) {
        if (!hashTable[str]) {
            teamPool[++teamCnt] = new Team();
            teamPool[teamCnt]->name = str;
            hashTable[str] = teamCnt;
            teamTable.insert((Data){teamCnt});
            cout << "[Info]Add successfully.\n";
            return;
        }
        cout << "[Error]Add failed: duplicated team name.\n";
        return;
    }
    cout << "[Error]Add failed: competition has started.\n";
}

void Flush() {
    int tmpCnt = 0;
    rkTable.clear();
    for (const auto& i : teamTable)
        teamPool[i.pos]->rk = ++tmpCnt, rkTable.push_back(i);
}

void Start(int duration1, int pbCnt1) {
    if (startFlag) {
        cout << "[Error]Start failed: competition has started.\n";
        return;
    }
    duration = duration1, pbCnt = pbCnt1, startFlag = true;
    Flush();
    cout << "[Info]Competition starts.\n";
}

void Submit(char pbName, const string& teamName, typeSta sta, int time) {
    Team* nowTeam = teamPool[hashTable[teamName]];
    nowTeam->pllast[pbName-'A'] = nowTeam->llast = nowTeam->plast[pbName-'A'][sta] = nowTeam->last[sta] = (Submission){pbName, sta, time};
    if (!fzFlag) {
        teamTable.erase((Data){hashTable[teamName]});
        nowTeam->pb[pbName-'A'].cnt++;
        if (!nowTeam->pb[pbName-'A'].firstAC) {
            if (sta == AC) nowTeam->pb[pbName-'A'].firstAC = time, nowTeam->accnt++, nowTeam->plty+=20*nowTeam->pb[pbName-'A'].fail+time;
            else nowTeam->pb[pbName-'A'].fail++;
        }
        teamTable.insert((Data){hashTable[teamName]});
    }
    else {
        nowTeam->fzpb[pbName-'A'].cnt++;
        if (!nowTeam->fzpb[pbName-'A'].firstAC) {
            if (sta == AC) nowTeam->fzpb[pbName-'A'].firstAC = time;
            else nowTeam->fzpb[pbName-'A'].fail++;
        }
    }
}

void Freeze() {
    if (fzFlag) {
        cout << "[Error]Freeze failed: scoreboard has been frozen.\n";
        return;
    }
    fzFlag = true;
    for (int i = 1; i <= teamCnt; ++i) {
        for (int j = 0; j < pbCnt; ++j)
            teamPool[i]->fzpb[j] = teamPool[i]->pb[j];
    }
    cout << "[Info]Freeze scoreboard.\n";
}


void QueryRank(const string& name) {
    Team* nowTeam = teamPool[hashTable[name]];
    if (nowTeam == nullptr) cout << "[Error]Query ranking failed: cannot find the team.\n";
    else {
        cout << "[Info]Complete query ranking.\n";
        if (fzFlag) cout << "[Warning]Scoreboard is frozen. The ranking may be inaccurate until it were scrolled.\n";
        cout << name << ' ' << "NOW AT RANKING " << nowTeam->rk << '\n';
    }
}

void Print() {
    int tmpCnt = 0;
    for (auto i : rkTable) {
        cout << teamPool[i.pos]->name << ' ' << ++tmpCnt << ' ' << teamPool[i.pos]->accnt << ' ' << teamPool[i.pos]->plty;
        for (int j = 0; j < pbCnt; ++j) {
            cout << ' ';
            if (teamPool[i.pos]->pb[j].cnt == teamPool[i.pos]->fzpb[j].cnt || teamPool[i.pos]->pb[j].firstAC) {
                if (teamPool[i.pos]->pb[j].firstAC) {
                    if (!teamPool[i.pos]->pb[j].fail) cout << '+';
                    else cout << '+' << teamPool[i.pos]->pb[j].fail;
                } else {
                    if (!teamPool[i.pos]->pb[j].fail) cout << '.';
                    else cout << '-' << teamPool[i.pos]->pb[j].fail;
                }
            }
            else {
                if (!teamPool[i.pos]->pb[j].fail) cout << '0';
                else cout << '-' << teamPool[i.pos]->pb[j].fail;
                cout << '/' << teamPool[i.pos]->fzpb[j].cnt-teamPool[i.pos]->pb[j].cnt;
            }
        }
        cout << '\n';
    }
}

void Scroll() {
    if (!fzFlag) {
        cout << "[Error]Scroll failed: scoreboard has not been frozen.\n";
        return;
    }
    cout << "[Info]Scroll scoreboard.\n";
    fzFlag = false;
    Flush();
    Print();
    while (true) {
        bool fireFlag = false;
        auto i = rkTable.end()-1;
        for (; i >= rkTable.begin(); --i) {
            Team* nowTeam = teamPool[i->pos];
            for (int j = 0; j < pbCnt; ++j) {
                if (nowTeam->pb[j].cnt != nowTeam->fzpb[j].cnt && !nowTeam->pb[j].firstAC) {
                    fireFlag = true;
                    nowTeam->pb[j] = nowTeam->fzpb[j];
                    if (nowTeam->pb[j].firstAC) {
                        nowTeam->accnt++;
                        nowTeam->plty += 20*nowTeam->pb[j].fail+nowTeam->pb[j].firstAC;
                    }
                    auto ii = upper_bound(rkTable.begin(), i, *i);
                    if(ii != i) {
                        cout << nowTeam->name << ' ' << teamPool[ii->pos]->name << ' ' << nowTeam->accnt << ' ' << nowTeam->plty << '\n';
                        for (auto iii = i; iii > ii; --iii) swap(*iii, *(iii-1));
                    }
                    break;
                }
            }
            if (fireFlag) break;
        }
        if (!fireFlag) break;
    }
    Print();
    teamTable.clear();
    for (int i = 0; i < rkTable.size(); ++i) teamTable.insert(rkTable[i]), teamPool[rkTable[i].pos]->rk = i+1;
}

void QuerySubmit(const string& teamName, char pbName, typeSta sta) {
    Team* nowTeam = teamPool[hashTable[teamName]];
    if (nowTeam == nullptr) {
        cout << "[Error]Query submission failed: cannot find the team.\n";
        return;
    }
    if (pbName == '$') {
        if (sta == ALL) {
            if (!nowTeam->llast.time) cout << "[Info]Complete query submission.\n" << "Cannot find any submission.\n";
            else cout << "[Info]Complete query submission.\n" << teamName << ' ' << nowTeam->llast.pbName << ' ' << stainfo[nowTeam->llast.statype] << ' ' <<nowTeam->llast.time << '\n';
        }
        else {
            if (!nowTeam->last[sta].time) cout << "[Info]Complete query submission.\n" << "Cannot find any submission.\n";
            else cout << "[Info]Complete query submission.\n" << teamName << ' ' << nowTeam->last[sta].pbName << ' ' << stainfo[sta] << ' ' <<nowTeam->last[sta].time << '\n';
        }
    }
    else {
        if (sta == ALL) {
            if (!nowTeam->pllast[pbName-'A'].time) cout << "[Info]Complete query submission.\n" << "Cannot find any submission.\n";
            else cout << "[Info]Complete query submission.\n" << teamName << ' ' << pbName << ' ' << stainfo[nowTeam->pllast[pbName-'A'].statype] << ' ' <<nowTeam->pllast[pbName-'A'].time << '\n';
        }
        else {
            if (!nowTeam->plast[pbName-'A'][sta].time) cout << "[Info]Complete query submission.\n" << "Cannot find any submission.\n";
            else cout << "[Info]Complete query submission.\n" << teamName << ' ' << pbName << ' ' << stainfo[sta] << ' ' <<nowTeam->plast[pbName-'A'][sta].time << '\n';
        }
    }
}

inline int StringToInt(const std::string& str) {
    int ret = 0;
    for (char ch : str) ret = ret * 10 + ch - '0';
    return ret;
}

int main() {
    string cmd, argv[233];
    int argc;
    while (getline(cin, cmd)) {
        argc = 0;
        int ii = 0, jj, len = cmd.size();
        //while ((cmd[ii] == ' ' || cmd[ii] == '\t') && ii < cmd.size())  ++ii;
        //while (cmd[len-1] == ' ' || cmd[len-1] == '\t') --len;
        for (jj = ii; ii < len; ++ii) {
            if (cmd[ii] == ' ') {
                argv[argc++] = cmd.substr(jj, ii-jj);
                jj = ii + 1;
            }
        }
        if (jj < len) argv[argc++] = cmd.substr(jj, len-jj);
        if (argc == 2 && argv[0] == "ADDTEAM") AddTeam(argv[1].c_str());
        else if (argc == 5 && argv[0] == "START") Start(StringToInt(argv[2]), StringToInt(argv[4]));
        else if (argc == 8 && argv[0] == "SUBMIT") {
            if (argv[5] == stainfo[0]) Submit(argv[1][0], argv[3], AC, StringToInt(argv[7]));
            else if (argv[5] == stainfo[1]) Submit(argv[1][0], argv[3], WA, StringToInt(argv[7]));
            else if (argv[5] == stainfo[2]) Submit(argv[1][0], argv[3], RE, StringToInt(argv[7]));
            else if (argv[5] == stainfo[3]) Submit(argv[1][0], argv[3], TLE, StringToInt(argv[7]));
        }
        else if (argc == 1 && argv[0] == "FLUSH") {
            Flush();
            cout << "[Info]Flush scoreboard.\n";
        }
        else if (argc == 1 && argv[0] == "FREEZE") Freeze();
        else if (argc == 1 && argv[0] == "SCROLL") Scroll();
        else if (argc == 2 && argv[0] == "QUERY_RANKING") QueryRank(argv[1]);
        else if (argc == 6 && argv[0] == "QUERY_SUBMISSION") {
            if (argv[3].size() < 8 || argv[5].size() < 7) continue;
            if(argv[3].substr(8,argv[3].size()-8) == "ALL") argv[3]="PROBLEM=$";
            argv[5] = argv[5].substr(7, argv[5].size()-7);
            if (argv[5] == stainfo[0]) QuerySubmit(argv[1], argv[3][8], AC);
            else if (argv[5] == stainfo[1]) QuerySubmit(argv[1], argv[3][8], WA);
            else if (argv[5] == stainfo[2]) QuerySubmit(argv[1], argv[3][8], RE);
            else if (argv[5] == stainfo[3]) QuerySubmit(argv[1], argv[3][8], TLE);
            else if (argv[5] == stainfo[4]) QuerySubmit(argv[1], argv[3][8], ALL);
        }
        else if (argc == 1 && argv[0] == "END") {
            startFlag = false;
            cout << "[Info]Competition ends.\n";
            //for (int i = 1; i <= teamCnt; ++i) delete teamPool[i];
            return 0;
        }
    }
    return 0;
}