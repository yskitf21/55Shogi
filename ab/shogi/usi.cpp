#include "usi.h"
#include <bitset>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

// コンストラクタ
USI::USI() {
    // 初期盤面を設定する
    root = Root();
    // 初期盤面を「訪れた盤面」に追加する
    visited_hash_keys.push_back(root.pos.get_hash_key());
}
std::ofstream logFile("log.txt"); // ログファイルを上書きモードで開く

void USI::loop() {
    StringEx cmd;

    while (std::getline(std::cin, cmd)) {
        logFile << "<< " << cmd
                << std::endl; // コマンドをログファイルに書き出す
        std::vector<std::string> cmds = cmd.Split();
        size_t len = cmds.size();

        if (cmds[0] == "quit") {
            break;
        }

        else if (cmds[0] == "usi") {
            usi();
        }

        else if (cmds[0] == "isready") {
            isready();
        }
        // Stateの初期盤面を設定する

        else if (cmds[0] == "usinewgame")
            continue;

        else if (cmds[0] == "position") {
            position(cmds[len - 1]);
            std::cout << "ok" << std::endl;
        }

        else if (cmds[0] == "go") {
            go();
        }

        else if (cmds[0] == "test") {
            test();
        }

        else if (cmds[0] == "display") {
            root.pos.display_bitboards();
            root.pos.display_hands();
        }

        // コマンドで"hello"と入力された場合
        else if (cmds[0] == "hello") {
            // こちらも"Hello!"と出力する
            // あいさつは大事
            std::cout << "Hello!" << std::endl;
        }

        else {
            std::cout << "invalid comannd: " + cmd << std::endl;
            logFile << "invalid comannd: " + cmd << std::endl;
        }
    }

    logFile.close(); // ログファイルを閉じる
}

void USI::usi() {
    send_id(ID_NAME);
    send_id(ID_AUTHOR);
    send_usiok();
}

void USI::isready() { send_readyok(); }

// commandの末尾を受け取って、Stateの盤面を更新する
void USI::position(std::string sfen) {
    // 初期盤面のコマンドは無視する
    if (sfen == "1") {
        return;
    }
    Move move = Move(sfen);
    root.pos.set_captured_piece(move);
    USI::do_move(move);
}

void USI::go() { send_bestmove(); }

void USI::send_id(ID id) {
    if (id == ID_NAME)
        std::cout << "id name " + ENGINE_NAME << std::endl;
    else if (id == ID_AUTHOR)
        std::cout << "id author " + ENGINE_AUTHOR << std::endl;
}

void USI::send_usiok() {
    std::string ret = "usiok";
    logFile << ">> " << ret << std::endl;
    std::cout << ret << std::endl;
}

void USI::send_readyok() {
    std::string ret = "readyok";
    logFile << ">> " << ret << std::endl;
    std::cout << ret << std::endl;
}

void USI::send_bestmove() {
    Move best_move = root.search();
    std::cout << "bestmove " << best_move << std::endl;

    if (best_move.is_resign()) {
        return;
    }

    USI::do_move(best_move);

    // "bestmove"を出力した時点で、将棋GUIにより強制的に標準出力を停止させられるので
    // 一番最後に実行するようにする
    // logFile << "bestmove " << best_move << std::endl;
}

void USI::do_move(Move move) {
    root.pos.do_move(move);
    visited_hash_keys.push_back(root.pos.get_hash_key());
}

// テスト用
void USI::test() {
    // Move best_move = Move(Move::NONE);
    // while (!best_move.is_resign()) {
    //     Move best_move = root.search();
    //     std::cout << best_move << std::endl;
    //     if (best_move.is_resign()) {
    //         break;
    //     }
    //     USI::do_move(best_move);
    // }
    // std::cout << "game finished" << std::endl;
    while (true) {
        go();
    }
}