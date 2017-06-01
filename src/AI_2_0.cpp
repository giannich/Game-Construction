#include "AI_2_0.hpp"
#include "Boat.hpp"

AI_2_0::AI_2_0(Track *t, int my_player, int num_boats, char* inFile, char* outFile, float randomness)
{
	track = t;
	myPlayer = my_player;
	numBoats = num_boats;
	std::ifstream myfile;
	myfile.open (inFile);
	size[0] = 170;
	size[1] = 50;
	size[2] = 10;
	size[3] = 1;

	for(int l = 0; l < 3; l++)
	{
		weights[l] = new float*[size[l+1]];
		for(int i = 0; i < size[l+1]; i++)
			weights[l][i] = new float[size[l]+1];
	}

	for(int l = 0; l < 3; l++)
		for(int i = 0; i < size[l+1]; i++)
			for(int j = 0; j < size[l]+1; j++)
				myfile >> weights[l][i][j];

	myfile.close();
	if(outFile!=nullptr)
	{
		out.open(outFile);
		isOutEnabled = 1; 
	}
	else
		isOutEnabled = 0;
	for(int l = 0; l < 4; l++)
	{
		layers[l] = new float[size[l]+1];
		layers[l][size[l]] = 1;
	}
	this->randomness = randomness;
	//std::cout << weights[1][5][15]<<std::endl;
}
InputState AI_2_0::getCommand(std::vector<Boat>& boats, std::vector<Soul>& souls)
{

	//0-99: 4(l_x,l_y,r_x,r_y)*25
	//100-163 : 8(active,x,y,vx,vy,theta,avel,N-segPos)*8 (my,first_other,3_ahead, 3_back)
	//164-169 : input
	float myseg = boats[myPlayer].segPosition;
	if(myseg<5 || myseg >= track->N - 20)
	{
		for(int i = 0; i < 25; i++)
		{
			int j = myseg + i - 5;
			if(j<0)
				j = 0;
			else if(j >= track->N)
				j = track->N - 1;
			layers[0][i*4] = track->l[j].x;
			layers[0][i*4+1] = track->l[j].y;
			layers[0][i*4+2] = track->r[j].x;
			layers[0][i*4+3] = track->r[j].y;

		}
	}
	else
	{
		for(int i = 0; i < 25; i++)
		{
			int j = myseg + i - 5;
			layers[0][i*4] = track->l[j].x;
			layers[0][i*4+1] = track->l[j].y;
			layers[0][i*4+2] = track->r[j].x;
			layers[0][i*4+3] = track->r[j].y;

		}
	}

	int pl[8];
	int sp[8];
	pl[0]=myPlayer;
	sp[0]=myseg;
	for(int i = 1; i < 8; i++)
	{
		pl[i] = -1;
		sp[i] = -1;
	}
	for(int i = 2; i < 5; i++)
		sp[i] = track->N + 1;
	for(int i = 0; i < boats.size(); i++)
	{
		if(i!=myPlayer)
		{
			if(boats[i].segPosition>sp[1])
			{
				pl[1]=i;
				sp[1]=boats[i].segPosition;
			}
			if(boats[i].segPosition >= myseg)
			{
				if(boats[i].segPosition < sp[2])
				{
					sp[4]=sp[3];
					sp[3]=sp[2];
					pl[4]=pl[3];
					pl[3]=pl[2];
					sp[2]=boats[i].segPosition;
					pl[2]=i;
				}
				else if(boats[i].segPosition < sp[3])
				{
					sp[4]=sp[3];
					pl[4]=pl[3];
					sp[3]=boats[i].segPosition;
					pl[3]=i;
				}
				else if(boats[i].segPosition < sp[4])
				{
					sp[4]=boats[i].segPosition;
					pl[4]=i;
				}
			}
			else
			{
				if(boats[i].segPosition > sp[5])
				{
					sp[7]=sp[6];
					sp[6]=sp[5];
					pl[7]=pl[6];
					pl[6]=pl[5];
					sp[5]=boats[i].segPosition;
					pl[5]=i;
				}
				else if(boats[i].segPosition > sp[6])
				{
					sp[7]=sp[6];
					pl[7]=pl[6];
					sp[6]=boats[i].segPosition;
					pl[6]=i;
				}
				else if(boats[i].segPosition > sp[7])
				{
					sp[7]=boats[i].segPosition;
					pl[7]=i;
				}
			}
		}
	}
	for(int i = 0; i < 8; i++)
	{
		if(pl[i]==-1)
		{
			layers[0][100+i*8] = 0;
			layers[0][100+i*8+1] = 0;
			layers[0][100+i*8+2] = 0;
			layers[0][100+i*8+3] = 0;
			layers[0][100+i*8+4] = 0;
			layers[0][100+i*8+5] = 0;
			layers[0][100+i*8+6] = 0;
			layers[0][100+i*8+7] = 0;
		}
		else
		{
			layers[0][100+i*8] = 1;
			layers[0][100+i*8+1] = boats[pl[i]].rigidBody->GetPosition().x;
			layers[0][100+i*8+2] = boats[pl[i]].rigidBody->GetPosition().y;
			layers[0][100+i*8+3] = boats[pl[i]].rigidBody->GetLinearVelocity().x;
			layers[0][100+i*8+4] = boats[pl[i]].rigidBody->GetLinearVelocity().y;
			layers[0][100+i*8+5] = boats[pl[i]].rigidBody->GetAngle();
			layers[0][100+i*8+6] = boats[pl[i]].rigidBody->GetAngularVelocity();
			layers[0][100+i*8+7] = track->N - boats[pl[i]].segPosition;
		}
	}


	float maxValue = -999999;
	int I,J;
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
		{
			for(int k = 164; k < 170; k++)
				layers[0][k] = 0;
			layers[0][164+i] = 1;
			layers[0][167+i] = 1;
			float value = getNNout();
			if(maxValue < value || (i==0 && j==0))
			{
				I = i;
				J = j;
				maxValue = value;
			}
		}
	srand(time(NULL));
	if(rand()/(float)RAND_MAX<randomness)
	{
		I = rand()%3;
		J = rand()%3;
	}
	if(isOutEnabled)
	{
		for(int i = 0; i < size[0]-6; i++)
		{
			out << layers[0][i]<< " ";
		}
		out << (int)(I==0) << " ";
		out << (int)(I==1) << " ";
		out << (int)(I==2) << " ";
		out << (int)(J==0) << " ";
		out << (int)(J==1) << " ";
		out << (int)(J==2);
		out << "\n";
		out.flush();
		//std::cout<<maxValue;
	}
	InputState is;
	if(I==0)
	{
		is.turn = Left;
	}
	else if(I==1)
	{
		is.turn = Neutral;
	}
	else
	{
		is.turn = Right;
	}
	if(J==0)
	{
		is.acc = Accelerating;
	}
	else if(J==1)
	{
		is.acc = Idling;
	}
	else
	{
		is.acc = Reversing;
	}
	return is;
}
float AI_2_0::getNNout()
{
	for(int l = 1; l < 3; l++)
		for(int i = 0; i < size[l]; i++)
		{
			layers[l][i] = 0;
			for(int j = 0; j < size[l-1] + 1; j++)
			{
				layers[l][i]+=layers[l-1][j]*weights[l-1][i][j];
			}
			layers[l][i] = tanh(layers[l][i]);
		}
	int l=3;
	for(int i = 0; i < size[l]; i++)
	{
		layers[l][i] = 0;
		for(int j = 0; j < size[l-1] + 1; j++)
		{
			layers[l][i]+=layers[l-1][j]*weights[l-1][i][j];
		}
	}
	return layers[3][0];
}
AI_2_0::~AI_2_0()
{
	if(isOutEnabled)
	{
		out.flush();
		out.close();
	}
	for(int l = 0; l < 3; l++)
	{
		for(int i = 0; i < size[l+1]; i++)
			delete [] weights[l][i];

		delete [] weights[l];
	}
	for(int l = 0; l < 4; l++)
	{
		delete [] layers[l];
	}
}