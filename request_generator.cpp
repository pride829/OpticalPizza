/*
request_generator.cpp必須要能夠輸入n = request的個數。接著他會產生n筆虛擬請求，並且輸入到virtual_request.txt中。

這其中每一筆資料都會先由四個時間參數開始，time_arrived, time_started, delta_time還有time_ended。
以目前的實驗環境，time_arrived先一律設成0。time_ended必須大於time_started，delta_time必須小於或等於time-ended - time_started。
請求必須按照time_started的大小排序。
這四種時間的上限都為1000。

接下來會有s行虛擬節點和其容量大小。s代表虛擬節點的個數，每一行的數字則是代表這個虛擬節點的容量為多少。因此，必須要有一個參數S代表每一筆虛擬請求可能具有節點個數的範圍(2 <= s <= S)。再來要有一個值C代表各個點之間容量c可能的範圍，1 <= c <= C。

接著換行，輸出一個-1。

接下來會有i行虛擬鏈路和其FSC(頻譜槽使用數量)。每一個虛擬鏈路都會有一個起始點和終點。不能有起始點和終點一樣的兩條線路！目前還不確定，不過為了安全起見也先不要有頭尾相接的兩條鏈路(例如說01跟10)。
每一個virtual request最多會有(s)(s-1)/2條鏈路，最短則是1。這個數值可以完全隨機，不必使用者輸入。
除此之外還要設定FSC的範圍為何。輸入一個值F代表每筆請求最高的FSC範圍，最小以10單位。因此每筆請求都會有一個FSC，10 <= FSC < F。

接著換行，輸出一個-1。重複以上直到n個虛擬請求產生完。

範例：
input: n=2, S = 4, F = 100, C = 10

output:

0 2 10 20
3
8
7
-1
2 1 20
1 0 30
0 2 40
-1
0 3 15 30
8
7
6
4
-1
0 1 90
1 2 50
0 2 80
0 3 40
-1
*/

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fstream >
#include <iomanip>
using namespace std;


void print_UI();

int main(){
	int k,j,z,tmp;
    unsigned seed;
    seed = (unsigned)time(NULL); // 取得時間序列
    srand(seed); 				 // 以時間序列當亂數種子
    
    //**輸入參數 ///////////////////////////////////////////////
    int T;  					// 設定時間上限 
	int n;			 			// n = request的個數
	int S;			 			// 代表每一筆虛擬請求可能具有節點個數的範圍
	int BW;		 	 			// 代表每筆請求最高的頻譜寬度範圍
	int C;				    	// 各個點之間容量c可能的範圍
	////////////////////////////////////////////////////////////
	
	int time_arrived , time_started , delta_time , time_ended;		//時間 
	int time_sort[10000] = {1000000};
	int s;			 			// 代表虛擬節點的個數
	int c;		 				// 表各個點之間容量c可能的範圍
	int i;		  				// 會有 i 行虛擬鏈路和其FSC(頻譜槽使用數量)
	int path_low,path_high;
	
		
	ofstream outfile("virtual_requests.txt"); // 宣告及開檔 
	print_UI();
	cin >> T >> n >> S >> BW >> C;			 //	*** 輸入參數 *** 
    time_arrived = 0; 					 // 初始請求到達時間 
    
	///////////time_started 時間排序 /////////////////
    for(k = 0 ; k < n ; k++){
    	time_sort[k] = rand() % (T - 1);
	}
	
	for(z=0;z<n;z++){
       for(j=0;j<n-z-1;j++){
            if(time_sort[j]>time_sort[j+1]){
            	tmp=time_sort[j];
                time_sort[j]=time_sort[j+1];
                time_sort[j+1]=tmp;
          	}
      	}
 	 }
	////////////////////////////////////////////////////
	
	
	for(k = 0 ; k < n ; k++){ 			 //  共n筆 
		s = rand()%(S - 2  +1) + 2; 	 //  節點個數 s ->  (2 <= s <= S) 
		path_low = s - 1; 					 //  每一個virtual request最少會有 1條 條鏈路  *(s - 1條)
		path_high = (s * ( s - 1 )) / 2; //  每一個virtual request最多會有s * (s-1) / 2條 條鏈路 
		////////////////////   產生時間亂數   //////////////////////
		
		time_started = time_sort[k];
		time_ended = (rand() % (T - time_started)) + time_started + 1;
		delta_time = (rand() % (time_ended - time_started)) + 1;		 		  //  delta_time > 0 
		outfile << time_arrived <<" " << time_started << " " << delta_time << " " << time_ended << endl;
		
		////////////////////////////////////////////////////////////
		
		
		
		///////////////  產生 s 行虛擬節點和其容量大小 c    //////////////
		
		for(j = 0 ; j < s ; j++){ 		 
		   	c = rand() % C + 1;			 // 容量 , 範圍，1 <= c <= C  
		    outfile<< c << endl;
		}
		outfile<< -1 << endl;
		
		//////////////////////////////////////////////////////////////////
		
		
		/////////////////////     產生虛擬鍊路      ///////////////////////////////
		int path_num=0;	
		int count = 0;
		float bw_float;
		int bw; 										      // 頻譜槽使用數量 
		int num = (rand()%(path_high-path_low+1)) + path_low; // num 條鍊路  random值 , 每一個virtual request最多會有(s)(s-1)/2條鏈路，最短則是 1 //( s - 1 )
		//int num =  s-1;//DEBUG USE
		int from , destination; 							  // 虛擬鍊路的頭 , 尾 
		float array[num][3] = {0}; 							  // 存取虛擬鍊路之二微陣列 
		int check; 											  // 用來檢察重複 , 頭尾相接狀況  
		int check_graph_num = 0;
		int check_graph[1000] = {0}; 
		bool exist; 
		while (path_num != num) {
			exist = true;
			check = 1;
	
			if (check_graph_num != s) {
	
				if (path_num == 0) {
					do {
						from = rand() % s;
						destination = rand() % s;
						bw = rand() % BW;					  // BW的範圍 ,  0 < bw < BW
						bw_float = (rand() % 100) / 100.0;
					} while (from == destination || (bw == 0 && bw_float == 0));
					check_graph[check_graph_num] = from;
					check_graph[check_graph_num + 1] = destination;
					check_graph_num += 2;
				}
				else {
					do {
						from = check_graph[rand() % check_graph_num];
						do {
							destination = rand() % s;
							for (int i = 0; i < check_graph_num; i++) {
								if (destination == check_graph[i]) {
									exist = true;
									break;
								}
								else {
									exist = false;
								}
							}
						} while (exist);
						bw = rand() % BW;					  // BW的範圍 ,  0 < bw < BW
						bw_float = (rand() % 100) / 100.0;
					} while (from == destination || (bw == 0 && bw_float == 0));
					check_graph[check_graph_num] = destination;
					check_graph_num++;
				}
	
			}
			else {
				do {
					from = rand() % s;
					destination = rand() % s;
					bw = rand() % BW;					  // BW的範圍 ,  0 < bw < BW
					bw_float = (rand() % 100) / 100.0;
				} while (from == destination || (bw == 0 && bw_float == 0));
			}
			if (path_num == 0) {
				array[0][0] = from;
				array[0][1] = destination;
				array[0][2] = bw + bw_float;
				path_num++;
			}
			else {
				for (j = 0; j < path_num; j++) {
					if ((from == array[j][0] && destination == array[j][1]) || (from == array[j][1] && destination == array[j][0])) {
						check = 0;
						break;
					}
				}
				if (check == 1) {
					array[path_num][0] = from;
					array[path_num][1] = destination;
					array[path_num][2] = bw + bw_float;
					path_num++;
				}
			}


		}
		/*DEBUG USE
		outfile <<"**************************************************" <<endl;
		for(j = 0 ; j < check_graph_num ; j++){
			outfile <<" "<<setw(2) << check_graph[j];	
		}
			outfile <<endl <<"**************************************************" <<endl;
		*/
		for(j = 0 ; j < num ; j++){
			outfile << (int)array[j][0]<<" "<< (int)array[j][1]<<" " << setiosflags(ios::fixed)<<setprecision(2)<< array[j][2] << endl;	
		}
		if(k != n-1) outfile<< -1 << endl; 
		else outfile << -1; 
	}
	outfile.close();	//關檔
	cout<<endl<<endl;
	cout <<"			//////////////////////////////////" <<endl<<endl;
	cout <<"          		產生之參數 : "<<endl<<endl;
	cout <<"	        		時間上限 : "<< T <<endl;
	cout <<"	          		產生之虛擬需求個數 : "<< n <<endl;
	cout <<"	          		節點個數的範圍 : "<<S<<endl;
	cout <<"	          		最高的頻譜寬度範圍 : " << BW <<endl;
	cout <<"	          		虛擬節點容量範圍　: " << C << endl;
	
	cout<< "		        	Finish Program !" << endl<<endl;
	cout <<"			//////////////////////////////////" <<endl;
	system("pause");
	return 0;
}

void print_UI(){
		cout << "                      ----------------------------"<<endl;
		cout << "                     *  Virtual Network Generator  *             "<<endl;
		cout << "                      ----------------------------"<<endl;
		cout << "           ****************************************************" <<endl;
		cout << "           *                   - 依序輸入 -                   *" <<endl;
		cout << "           *                                                  *" <<endl;
		cout << "           *          1. 時間上限 ( T )                       *" <<endl;
		cout << "           *          2. Requset 數量 ( n )                   *" <<endl;
		cout << "           *          3. 可能具有節點個數的範圍 ( S )         *" <<endl;
		cout << "           *          4. 每筆請求最高的頻譜範圍 ( BW )        *" <<endl;
		cout << "           *          5. 個點之間容量可能的範圍 ( C )         *" <<endl;
		cout << "           *                                                  *" <<endl;
		cout << "           ****************************************************" <<endl<<endl;
		cout << "--------------------------------------------------------------------------------"<<endl;
		cout << "                 - 請輸入虛擬請求 ( T , n , S , BW , C ) -     " <<endl;
		cout << "                   Input : ";
}
