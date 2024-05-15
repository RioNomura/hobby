gameBoard = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24] # ゲームに利用する盤面

# 盤面を3x3で表示する
def displayBoard():
    # gameBoardの要素を1つずつ表示
    print("---+----+----+----+---") # デコレーション
    for i in range(0, len(gameBoard)):
        if len(str(gameBoard[i]))< 2:
            if(i%5 == 4): # 5回に1回は改行
                print(gameBoard[i]) # 改行する
                print("---+----+----+----+---") # デコレーション

            elif(i%5 == 1 or i%5 == 2):
                print("  | " + str(gameBoard[i]) , end="") # 改行しない
                
            elif(i%5 == 3):
                print("  | " + str(gameBoard[i]) + "  | "  , end="") # 改行しない
                
            else :
                print(gameBoard[i], end="") # 改行しない
        
        else:
            if(i%5 == 4): # 5回に1回は改行
                print(gameBoard[i]) # 改行する
                print("---+----+----+----+---") # デコレーション

            elif(i%5 == 1 or i%5 == 2):
                print(" | " + str(gameBoard[i]) , end="") # 改行しない
                
            elif(i%5 == 3):
                print(" | " + str(gameBoard[i]) + " | "  , end="") # 改行しない
                
            else :
                print(gameBoard[i], end="") # 改行しない

    print()

# ターンを進める
def inputBoard(playerType):
    # 1.座標を入力させる
    if turn < 3:
            if(playerType == "o"):  # o が渡されたら座標を入力
                tgt = int(input("辺の座標を入れてください: "))
                while True:
                    if 5 < tgt < 9 or 10 < tgt < 14 or 15 < tgt < 19:
                        tgt = int(input("辺の座標(0~5,9,10,14,15,19~24)を入れてください: "))
                    else :
                        break
            elif(playerType == "x"):  # x が渡されたら座標を入力
                tgt = int(input("辺の座標を入れてください: "))
                while True:
                    if 5 < tgt < 9 or 10 < tgt < 14 or 15 < tgt < 19:
                        tgt = int(input("辺の座標(0~5,9,10,14,15,19~24)を入れてください: "))
                    else :
                        break
            elif(playerType == "△"):  # o が渡されたら座標を入力
                tgt = int(input("12以外の座標を入れてください: "))
                while True:
                    if tgt == 12:
                        tgt = int(input("12以外の座標を入れてください: "))
                    else :
                        break

    else:
        if(playerType == "o"):  # o が渡されたら座標を入力
            tgt = int(input("0~24の座標を入れてください: "))
        elif(playerType == "x"):  # x が渡されたら座標を入力
            tgt = int(input("0~24の座標を入れてください: "))
        elif(playerType == "△"):  # △ が渡されたら座標を入力
            tgt = int(input("0~24の座標を入れてください: "))

    # 2.入力座標に 'o'か 'x' が入っていないことを確認
    if(gameBoard[tgt] == 'o' or gameBoard[tgt] == 'x' or gameBoard[tgt] == '△'):
        inputBoard(playerType)

    # 3.gameBoardに反映
    else:
        gameBoard[tgt] = playerType

# 勝利判定
def winner():
    # 勝ち手を列挙
    lines = [
      #横列の勝ち手
      [0, 1, 2],[1, 2, 3],[2, 3, 4],
      [5, 6, 7],[6, 7, 8],[7, 8, 9],
      [10, 11, 12],[11, 12, 13],[12, 13, 14],
      [15, 16, 17],[16, 17, 18],[17, 18, 19],
      [20, 21, 22],[21, 22, 23],[22, 23, 24],
      #縦列の勝ち手
      [0, 5, 10],[5, 10, 15],[10, 15, 20],
      [1, 6, 11],[6, 11, 16],[11, 16, 21],
      [2, 7, 12],[7, 12, 17],[12, 17, 22],
      [3, 8, 13],[8, 13, 18],[13, 18, 23],
      [4, 9, 14],[9, 14, 19],[14, 19, 24],
      #右下斜め列の勝ち手
      [2, 8, 14],[1, 7, 13],[7, 13, 19],
      [0, 6, 12],[6, 12, 18],[12, 18, 24],
      [5, 11, 17],[11, 17, 23],[10, 16, 22],
      #左下斜め列の勝ち手
      [2, 6, 10],[3, 7, 11],[7, 11, 15],
      [4, 8, 12],[8, 12, 16],[12, 16, 20],
      [9, 13, 17],[13, 17, 21],[14, 18, 22]
    ]
    # forで勝ち手を1パターンずつ見ていく
    for i in range(0, len(lines)):
        [a, b, c] = lines[i]
        # 勝ち手の場所に同じ記号が入っていないかを確認
        if gameBoard[a] and gameBoard[a] == gameBoard[b] and gameBoard[a] == gameBoard[c]: 
            # 同じ記号が入っていたら、入っている記号を返す
            return gameBoard[a]
    # どちらも勝っていない場合はNoneを返す
    return None

displayBoard() # 実行時に初めに表示される盤面
# ゲームを実行する
for turn in range(0,25):
    # 誰のターンかを判定する
    if(turn %3 == 0) : # 一人目のターン
        print("1人目(o)")
        inputBoard("o")
    elif(turn %3 == 1) : # 二人目のターン
        print("2人目(x)")
        inputBoard("x")
    else: # 二人目のターン
        print("3人目(△)")
        inputBoard("△")

    displayBoard() # 盤面表示

    if winner() == 'o' : # 勝敗判定
        print("1人目(o)の勝ちです!!")
        break

    elif winner() == 'x' : # 勝敗判定
        print("2人目(x)の勝ちです!!")
        break

    elif winner() == '△' : # 勝敗判定
        print("3人目(△)の勝ちです!!")
        break

    if turn == 24: # 8手目で引き分け
        print("全員譲らない! 引き分けです!")
        break
