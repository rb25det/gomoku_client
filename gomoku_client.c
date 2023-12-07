//http://blog.livedoor.jp/akf0/archives/51585502.html

#include <stdio.h>
#include <winsock2.h>

#define SIZE 15

// 盤の初期化(追加条項、削除不可)
void initializeBoard(int board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board[i][j] = 0;		//空いている場所を0に設定
        }
    }
}

//禁じ手判定(追加条項、削除不可)
int checkForbiddenMoves(int board[SIZE][SIZE], int player, int row, int col){
	int consecutiveCount;
	//三三禁
	
	//四四禁

	//長連
	// 横方向
    for (int i = col - 5; i <= col + 5; i++) {
        if (i < 0 || i >= SIZE) continue;
        if (board[row][i] == player) {
            consecutiveCount++;
            if (consecutiveCount == 6) return 1; // 勝利
        } else {
            consecutiveCount = 0;
        }
    }

    // 縦方向
    consecutiveCount = 0;
    for (int i = row - 5; i <= row + 5; i++) {
        if (i < 0 || i >= SIZE) continue;
        if (board[i][col] == player) {
            consecutiveCount++;
            if (consecutiveCount == 6) return 1; // 勝利
        } else {
            consecutiveCount = 0;
        }
    }

    // 斜め方向 (左上から右下)
    consecutiveCount = 0;
    for (int i = -5; i <= 5; i++) {
        int r = row + i;
        int c = col + i;
        if (r < 0 || r >= SIZE || c < 0 || c >= SIZE) continue;
        if (board[r][c] == player) {
            consecutiveCount++;
            if (consecutiveCount == 6) return 1; // 勝利
        } else {
            consecutiveCount = 0;
        }
    }

    // 斜め方向 (右上から左下)
    consecutiveCount = 0;
    for (int i = -5; i <= 5; i++) {
        int r = row + i;
        int c = col - i;
        if (r < 0 || r >= SIZE || c < 0 || c >= SIZE) continue;
        if (board[r][c] == player) {
            consecutiveCount++;
            if (consecutiveCount == 6) return 1; // 勝利
        } else {
            consecutiveCount = 0;
        }
    }

	return 0;	//禁じ手なし
}

// 引き分け判定(追加条項、削除不可)
int checkDraw(int board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == 0) {
                return 0; // ボードがまだ埋まっていない
            }
        }
    }
    return 1; // ボードが埋まっている
}

//勝利判定(追加条項、削除不可)
int checkWin(int board[SIZE][SIZE], int player, int row, int col) {
    int consecutiveCount = 0;

    // 横方向
    for (int i = col - 4; i <= col + 4; i++) {
        if (i < 0 || i >= SIZE) continue;
        if (board[row][i] == player) {
            consecutiveCount++;
            if (consecutiveCount == 5) return 1; // 勝利
        } else {
            consecutiveCount = 0;
        }
    }

    // 縦方向
    consecutiveCount = 0;
    for (int i = row - 4; i <= row + 4; i++) {
        if (i < 0 || i >= SIZE) continue;
        if (board[i][col] == player) {
            consecutiveCount++;
            if (consecutiveCount == 5) return 1; // 勝利
        } else {
            consecutiveCount = 0;
        }
    }

    // 斜め方向 (左上から右下)
    consecutiveCount = 0;
    for (int i = -4; i <= 4; i++) {
        int r = row + i;
        int c = col + i;
        if (r < 0 || r >= SIZE || c < 0 || c >= SIZE) continue;
        if (board[r][c] == player) {
            consecutiveCount++;
            if (consecutiveCount == 5) return 1; // 勝利
        } else {
            consecutiveCount = 0;
        }
    }

    // 斜め方向 (右上から左下)
    consecutiveCount = 0;
    for (int i = -4; i <= 4; i++) {
        int r = row + i;
        int c = col - i;
        if (r < 0 || r >= SIZE || c < 0 || c >= SIZE) continue;
        if (board[r][c] == player) {
            consecutiveCount++;
            if (consecutiveCount == 5) return 1; // 勝利
        } else {
            consecutiveCount = 0;
        }
    }

    return 0; // 勝利なし
}

int main(void) {	

	//接続するサーバの情報の構造体を用意

	struct sockaddr_in dest;

	memset(&dest, 0, sizeof(dest));

	//サーバの情報を入力

	char destination[100];  //アドレス
	int port;		//ポート番号

	printf("アドレスを入力してください");
	scanf("%s", destination);
	printf("ポート番号を入力してください");
	scanf("%d", &port);
	dest.sin_port = htons(port);
	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = inet_addr(destination);



	//ソケット通信の準備・生成
	WSADATA data;
	WSAStartup(MAKEWORD(2, 0), &data);
	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);

	//サーバへの接続
	if (connect(s, (struct sockaddr *) &dest, sizeof(dest))) {
		printf("%sに接続できませんでした\n", destination);
		return -1;

	}

	printf("%sに接続しました\n", destination);
	
	char msg[1024];
	char buffer[1024];
	int board[SIZE][SIZE];	//盤の作成
	initializeBoard(board);	//盤の初期化

	boolean isFirst = TRUE;	//名前入力かどうかの判定
	boolean Advance;		//先攻かどうか
	int row, col;			//打った手の位置
	int your = 1;			//自分の手を1に設定
	int com = 2;			//相手の手を２に設定

	//先攻か後攻か(追加条項、削除不可)
	if(port == 12345){
		Advance = TRUE;		//自身が先攻
	}else{
		Advance = FALSE;	//自身が後攻
	}
	
	//サーバからデータを受信
	recv(s, buffer, 1024, 0);
	printf("→ %s\n\n", buffer);

	while(1){
		char buffer2[1024];
		memset(buffer2, 0, sizeof(buffer2));
		
		if(isFirst){
			//名前の入力
			printf("送信メッセージを入力(qで終了)");
			scanf("%s", msg);
			if(msg[0] == 'q' || msg[0] == 'Q'){
				break;
			}
		}else{
			// 相手の手を代入(追加事項、削除不可)
			if(strcmp(buffer2, "start") != 0){
				sscanf(buffer2, "%d,%d", &row, &col);
				row--;
				col--;
				board[row][col] = com;
			}
			
			// 禁じ手の判断(追加事項、削除不可)
			if(Advance == FALSE && checkForbiddenMoves(board, com, row, col) == 1){
				const char Forbidden[1024] = "Yuor hands is Forbidden";
				send(s, Forbidden, strlen(Forbidden), 0);
				break;
			}

			// 引き分け判断(追加条項、削除不可)
			if (checkDraw(board)) {
				const char drow[1024] = "The game is a draw!";
				send(s, drow, strlen(drow), 0);
				break;
			}

			//自分の手を決定(今後変更)
			printf("送信メッセージを入力(qで終了)");
			scanf("%s", msg);
			if(msg[0] == 'q' || msg[0] == 'Q'){
				break;
			}

			//勝利判断(今後追加)
			

		}
		//サーバにデータを送信
		send(s, msg, strlen(msg), 0);

		//サーバからデータを受信
		recv(s, buffer2, 1024, 0);
		printf("→ %s\n\n", buffer2);
	}

	// Windows でのソケットの終了
	closesocket(s);
	WSACleanup();
	return 0;

}