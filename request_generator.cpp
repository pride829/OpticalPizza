/*
request_generator.cpp�����n�����Jn = request���ӼơC���ۥL�|����n�������ШD�A�åB��J��virtual_request.txt���C

�o�䤤�C�@����Ƴ��|���ѥ|�Ӯɶ��Ѽƶ}�l�Atime_arrived, time_started, delta_time�٦�time_ended�C
�H�ثe���������ҡAtime_arrived���@�߳]��0�Ctime_ended�����j��time_started�Adelta_time�����p��ε���time-ended - time_started�C
�ШD��������time_started���j�p�ƧǡC
�o�|�خɶ����W������1000�C

���U�ӷ|��s������`�I�M��e�q�j�p�Cs�N������`�I���ӼơA�C�@�檺�Ʀr�h�O�N��o�ӵ����`�I���e�q���h�֡C�]���A�����n���@�ӰѼ�S�N��C�@�������ШD�i��㦳�`�I�Ӽƪ��d��(2 <= s <= S)�C�A�ӭn���@�ӭ�C�N��U���I�����e�qc�i�઺�d��A1 <= c <= C�C

���۴���A��X�@��-1�C

���U�ӷ|��i���������M��FSC(�W�мѨϥμƶq)�C�C�@�ӵ���������|���@�Ӱ_�l�I�M���I�C���঳�_�l�I�M���I�@�˪�����u���I�ثe�٤��T�w�A���L���F�w���_���]�����n���Y���۱���������(�Ҧp��01��10)�C
�C�@��virtual request�̦h�|��(s)(s-1)/2������A�̵u�h�O1�C�o�Ӽƭȥi�H�����H���A�����ϥΪ̿�J�C
�������~�٭n�]�wFSC���d�򬰦�C��J�@�ӭ�F�N��C���ШD�̰���FSC�d��A�̤p�H10���C�]���C���ШD���|���@��FSC�A10 <= FSC < F�C

���۴���A��X�@��-1�C���ƥH�W����n�ӵ����ШD���ͧ��C

�d�ҡG
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
    seed = (unsigned)time(NULL); // ���o�ɶ��ǦC
    srand(seed); 				 // �H�ɶ��ǦC��üƺؤl
    
    //**��J�Ѽ� ///////////////////////////////////////////////
    int T;  					// �]�w�ɶ��W�� 
	int n;			 			// n = request���Ӽ�
	int S;			 			// �N��C�@�������ШD�i��㦳�`�I�Ӽƪ��d��
	int BW;		 	 			// �N��C���ШD�̰����W�мe�׽d��
	int C;				    	// �U���I�����e�qc�i�઺�d��
	////////////////////////////////////////////////////////////
	
	int time_arrived , time_started , delta_time , time_ended;		//�ɶ� 
	int time_sort[10000] = {1000000};
	int s;			 			// �N������`�I���Ӽ�
	int c;		 				// ��U���I�����e�qc�i�઺�d��
	int i;		  				// �|�� i ���������M��FSC(�W�мѨϥμƶq)
	int path_low,path_high;
	
		
	ofstream outfile("virtual_requests.txt"); // �ŧi�ζ}�� 
	print_UI();
	cin >> T >> n >> S >> BW >> C;			 //	*** ��J�Ѽ� *** 
    time_arrived = 0; 					 // ��l�ШD��F�ɶ� 
    
	///////////time_started �ɶ��Ƨ� /////////////////
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
	
	
	for(k = 0 ; k < n ; k++){ 			 //  �@n�� 
		s = rand()%(S - 2  +1) + 2; 	 //  �`�I�Ӽ� s ->  (2 <= s <= S) 
		path_low = s - 1; 					 //  �C�@��virtual request�ַ̤|�� 1�� �����  *(s - 1��)
		path_high = (s * ( s - 1 )) / 2; //  �C�@��virtual request�̦h�|��s * (s-1) / 2�� ����� 
		////////////////////   ���ͮɶ��ü�   //////////////////////
		
		time_started = time_sort[k];
		time_ended = (rand() % (T - time_started)) + time_started + 1;
		delta_time = (rand() % (time_ended - time_started)) + 1;		 		  //  delta_time > 0 
		outfile << time_arrived <<" " << time_started << " " << delta_time << " " << time_ended << endl;
		
		////////////////////////////////////////////////////////////
		
		
		
		///////////////  ���� s ������`�I�M��e�q�j�p c    //////////////
		
		for(j = 0 ; j < s ; j++){ 		 
		   	c = rand() % C + 1;			 // �e�q , �d��A1 <= c <= C  
		    outfile<< c << endl;
		}
		outfile<< -1 << endl;
		
		//////////////////////////////////////////////////////////////////
		
		
		/////////////////////     ���͵������      ///////////////////////////////
		int path_num=0;	
		int count = 0;
		float bw_float;
		int bw; 										      // �W�мѨϥμƶq 
		int num = (rand()%(path_high-path_low+1)) + path_low; // num �����  random�� , �C�@��virtual request�̦h�|��(s)(s-1)/2������A�̵u�h�O 1 //( s - 1 )
		//int num =  s-1;//DEBUG USE
		int from , destination; 							  // ����������Y , �� 
		float array[num][3] = {0}; 							  // �s������������G�L�}�C 
		int check; 											  // �Ψ��˹�� , �Y���۱����p  
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
						bw = rand() % BW;					  // BW���d�� ,  0 < bw < BW
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
						bw = rand() % BW;					  // BW���d�� ,  0 < bw < BW
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
					bw = rand() % BW;					  // BW���d�� ,  0 < bw < BW
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
	outfile.close();	//����
	cout<<endl<<endl;
	cout <<"			//////////////////////////////////" <<endl<<endl;
	cout <<"          		���ͤ��Ѽ� : "<<endl<<endl;
	cout <<"	        		�ɶ��W�� : "<< T <<endl;
	cout <<"	          		���ͤ������ݨD�Ӽ� : "<< n <<endl;
	cout <<"	          		�`�I�Ӽƪ��d�� : "<<S<<endl;
	cout <<"	          		�̰����W�мe�׽d�� : " << BW <<endl;
	cout <<"	          		�����`�I�e�q�d��@: " << C << endl;
	
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
		cout << "           *                   - �̧ǿ�J -                   *" <<endl;
		cout << "           *                                                  *" <<endl;
		cout << "           *          1. �ɶ��W�� ( T )                       *" <<endl;
		cout << "           *          2. Requset �ƶq ( n )                   *" <<endl;
		cout << "           *          3. �i��㦳�`�I�Ӽƪ��d�� ( S )         *" <<endl;
		cout << "           *          4. �C���ШD�̰����W�нd�� ( BW )        *" <<endl;
		cout << "           *          5. ���I�����e�q�i�઺�d�� ( C )         *" <<endl;
		cout << "           *                                                  *" <<endl;
		cout << "           ****************************************************" <<endl<<endl;
		cout << "--------------------------------------------------------------------------------"<<endl;
		cout << "                 - �п�J�����ШD ( T , n , S , BW , C ) -     " <<endl;
		cout << "                   Input : ";
}
