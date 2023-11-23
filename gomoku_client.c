//http://blog.livedoor.jp/akf0/archives/51585502.html

#include <stdio.h>
#include <winsock2.h>

#define SIZE 15

// 盤の初期化(追加条項、削除不可)
void initializeBoard(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board[i][j] = '0';
        }
    }
}

// 引き分け判定(追加条項、削除不可)
int checkDraw(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == ' ') {
                return 0; // ボードがまだ埋まっていない
            }
        }
    }
    return 1; // ボードが埋まっている
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
	char board[SIZE][SIZE];
	initializeBoard(board);
	boolean isFirst = TRUE;
	int row, col;
	
	//サーバからデータを受信
	recv(s, buffer, 1024, 0);
	printf("→ %s\n\n", buffer);

	while(1){
		char buffer2[1024];
		memset(buffer2, 0, sizeof(buffer2));
		
		// 相手の手を代入(追加事項、削除不可)
		if(isFirst){
			sscanf(buffer, "%d,%d", &row, &col);
			isFirst = FALSE;
		}else{
			sscanf(buffer2, "%d,%d", &row, &col);
		}
		row--;
		col--;
		board[row][col] = '1';

		// 禁じ手の判断(追加事項、削除不可)

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