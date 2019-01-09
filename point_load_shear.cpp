//Author:Der Zhi Jeremy Sow
//Date: 20/11/18

#include "pch.h"
#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;


class BeamProperties {			//beamdiagram class (only for drawing and storing).
protected:										//inheritance used, hence protected
	double beamlength;							//beam's length
	char beamDraw[25][200];						//beam array
	int scale_factor = 5;						//scales the beam in the diagram to the actual length.
	vector <double> store_point_force_magnitude;			//MAGNITUDE of point force.
	vector <double> store_point_force_location;				// stores the laod force location.
	vector <double> store_load_force_magnitude_per_meter;				//MAGNITUDE of load force. in N/m
	vector <double> store_load_force_magnitude;
	vector <double> load_force_location;
	vector <double> store_load_force_location_begin;		//stores the load force location.
	vector <double> store_load_force_location_end;			//store the load force location
	vector <double> store_load_force_location_centroid;		//centroid of the force
	vector <double> store_support_location;					//LOCATION of support.
	vector <double> store_support_magnitude;
	vector <double> store_moments_point_force;
	vector <double> store_moments_load_force;
	vector <double> store_force;							//store the force magnitude
	vector <double> store_location;							//store the vector magnitude

public:

	void initBeam() {												//draw the beam for the user.
		for (int row = 0; row < 25; row++) {
			for (int column = 0; column < 200; column++){
			cout << beamDraw[row][column];
			}
		cout << endl;
		}
	}

	void setBeam() {												//set beam length.
		for (int row = 0; row < 25; row++) {
			for (int column = 0; column < 200; column++) {
				beamDraw[row][column] = ' ';
			}
		}
		cout << "enter beam length" << endl;
		cin >> beamlength;
		while (beamlength > 38) {
			cout << "the beam has a maximum length of 38 m only. Enter again" << endl;
			cin >> beamlength;
		}
		for (int column = 0; column <= beamlength * scale_factor; column++) {
			beamDraw[12][column] = '=';
		}
	}

	void setForces() {								//draws forces.
		while (1) {
			double force_location; char force_direction; char escape; char force_type; double force_value; //beam properties for drawing
			cout << "enter force type. p for point load, l for uniform distributed load" << endl;
			cin >> force_type;
			while ((force_type != 'p') && (force_type != 'l')) {
				cout << "invalid ouput" << endl;
				cin >> force_type;
			}

			if (force_type == 'p') {													// point load.
				cout << "Enter force location." << endl;								// force location.
				cin >> force_location;
				store_point_force_location.push_back(force_location);
				store_location.push_back(force_location);								// *NEW* store the force's location;
				while (force_location > beamlength) {
					cout << "invalid output" << endl;
					cin >> force_location;
				}

				cout << "enter force direction. Enter 'u' for upwards force, 'd' for downwards force." << endl; //force direction
				cin >> force_direction;
				while ((force_direction != 'u') && (force_direction != 'd')) {
					cout << "invalid ouput" << endl;
					cin >> force_direction;
				}
						
				if (force_direction == 'd') {									// downward force
					cout << "enter force magnitude in Newton. (postive only)" << endl;
					cin >> force_value;
					store_force.push_back(force_value*-1);						//NEW. store the force magnitude
					store_point_force_magnitude.push_back(force_value*-1);			//negative force.
					for (int row = 10; row >= 4; row--) {
						int beamlengthConvert = force_location * scale_factor;
						beamDraw[row][beamlengthConvert] = '|';
						beamDraw[11][beamlengthConvert] = 'v';
					}
				}

				if (force_direction == 'u') {									//upward force
					cout << "enter force magnitude in Newton. (postive only)" << endl;
					cin >> force_value;
					store_force.push_back(force_value);				//NEW.. store the force magnitude
					store_point_force_magnitude.push_back(force_value);		//store force
					for (int row = 14; row <= 18; row++) {
						int beamlengthConvert = force_location * scale_factor;
						beamDraw[row][beamlengthConvert] = '|';
						beamDraw[13][beamlengthConvert] = '^';
					}
				}
			}

			/////////////////////////////////////////////////////////////////////distributed force below//////////////////////////////////////////////////////////////////////////////
			if (force_type == 'l') {							//distributed load setting//
				double force_location_begin; double force_location_end; char force_direction_distributed;
				cout << " specify force location at left end (meters)." << endl;
				cin >> force_location_begin;								//force begin = left end. hence it should have a smaller value.

				while (force_location_begin > beamlength) {
					cout << "invalid output" << endl;
					cin >> force_location_begin;
				}

				cout << "specify force location at right end (meters)." << endl;
				cin >> force_location_end;
				while (force_location_end > beamlength) {
					cout << "invalid output" << endl;
					cin >> force_location_end;
				}

				cout << "forces direction. enter u for upwards, d for downwards." << endl;
				cin >> force_direction_distributed;

				if (force_direction_distributed == 'u') {		//upward force for distributed load
					cout << "enter load distribution in N/m" << endl;
					cin >> force_value;

				

				
				

					store_load_force_magnitude_per_meter.push_back(force_value);
					store_load_force_location_begin.push_back(force_location_begin);
					store_load_force_location_end.push_back(force_location_end);
					for (int row = 14; row <= 16; row++) {
						int ConvertForceBegin = force_location_begin * scale_factor;
						int ConvertForceEnd = force_location_end * scale_factor;
						beamDraw[row][ConvertForceBegin] = '|';
						beamDraw[13][ConvertForceBegin] = '^';
						beamDraw[row][ConvertForceEnd] = '|';
						beamDraw[13][ConvertForceEnd] = '^';
						for (int straightLine = ConvertForceBegin; straightLine <= ConvertForceEnd; straightLine++) {
							beamDraw[16][straightLine] = '-';
							beamDraw[13][straightLine] = '^';
						}
					}
				}

				if (force_direction_distributed == 'd') {		// downward force for distributed load
					cout << "enter load distribution in N/m" << endl;
					cin >> force_value;
					store_load_force_magnitude_per_meter.push_back(force_value*-1);
					for (int row = 10; row >= 7; row--) {
						int ConvertForceBegin = force_location_begin * scale_factor;
						int ConvertForceEnd = force_location_end * scale_factor;
						beamDraw[row][ConvertForceBegin] = '|';
						beamDraw[11][ConvertForceBegin] = 'v';
						beamDraw[row][ConvertForceEnd] = '|';
						beamDraw[11][ConvertForceEnd] = 'v';
						for (int straightLine = ConvertForceBegin; straightLine <= ConvertForceEnd; straightLine++) {
							beamDraw[7][straightLine] = '-';
						}
					}
				}
			}
			cout << "enter e to quit force setup and proceed to support setup, else press anything else." << endl;
			cin >> escape;
			if (escape == 'e') {
			break;
			}
		}
	}

	void setSupports() {
		double support_location; char escape; int InitSupport = 0;
		while (1) {
			cout << "enter your support locations." << endl;
			cin >> support_location;
			int supportConvert = support_location * scale_factor;
			while (beamDraw[13][supportConvert] == '^') {
				cout << "there is currently a force being placed here. Insert support again." << endl;
				cin >> support_location;
				supportConvert = support_location * scale_factor;				
			}
			store_force.push_back(InitSupport);								//NEW. STORE SUPPORT MAGNITUDE.
			store_location.push_back(support_location);						//NEW. STORE SUPPORT LOCATION.
			store_support_location.push_back(support_location);
			beamDraw[13][supportConvert] = 'O';
			if (store_support_location.size() == 2){
				break;
			}
		}
	}
};

class beamCalc:public BeamProperties{
public:
	void Compute_support_reaction() {
		double sum_moments_point_force = 0;				
		double sum_magnitude_point_force = 0;
		double sum_moments_load_force = 0;
		double sum_magnitude_load_force = 0;
		if (store_point_force_location.size() > 0) {					//checks for point force presence.
			for (int i = 0; i < store_point_force_location.size() ; i++) {		//loop through all point forces
				double pointForceMoments = store_point_force_magnitude[i] * (store_point_force_location[i] - store_support_location[0]);	//compute the moments
				store_moments_point_force.push_back(pointForceMoments);				//contains the moments for each point force into this vector
				sum_moments_point_force += store_moments_point_force[i];			//sums all the moments of eahc force
				sum_magnitude_point_force += store_point_force_magnitude[i];		//sums all the magnitude of forces 
			}
		}

		if (store_load_force_location_begin.size() > 0) {												//checks for load force presence.
			for (int i = 0; i < store_load_force_magnitude_per_meter.size(); i++) {						//for each looad force element
				double storeCentroid = 0.5*(store_load_force_location_begin[i] - store_load_force_location_end[i]);		//store the centroid value
				store_load_force_location_centroid.push_back(storeCentroid);							// stores the centroid of each element into this array
				double storeLoadForce = store_load_force_magnitude_per_meter[i] * (abs(store_load_force_location_centroid[i] / 0.5));	//force*distance for force value.location not calculated.												
				store_load_force_magnitude.push_back(storeLoadForce);									//stores each load force into this vector
				double storeLoadMoments = store_load_force_magnitude[i] * (store_load_force_location_centroid[i] - store_support_location[0]);		//store the load force moments
				store_moments_load_force.push_back(storeLoadMoments);		//stores all moments of each load forces into this array
				sum_moments_load_force += store_moments_load_force[i];		//adds all the moments load forces 		
				sum_magnitude_load_force += store_load_force_magnitude[i];	// adds the magnitude of all load forces
			}
		}

			double storeSupport = -(sum_moments_point_force + sum_moments_load_force) / (abs(store_support_location[1] - store_support_location[0])); //compute first support value.
			store_support_magnitude.push_back(storeSupport);					//store the supports in the vector
			storeSupport= -store_support_magnitude[0] - (sum_magnitude_point_force + sum_magnitude_load_force);  	//compute second support value.
			store_support_magnitude.push_back(storeSupport);	
			cout << "the support at " << store_support_location[1] << " meters is " << store_support_magnitude[0] << endl;			//prints out support value at [1] since [0] is stationary.
			for (int i = 0; i < store_force.size(); i++) {																//NEW. assign supportss to its values.
				if (store_force[i] == 0 && store_location[i] == store_support_location[1]) {
					store_force[i] = store_support_magnitude[0];
				}
				else if (store_force[i] == 0 && store_location[i] == store_support_location[0]) {
					store_force[i] = store_support_magnitude[1];
				}
					
			}
			cout << "the support at " << store_support_location[0] << " meters is " << store_support_magnitude[1] << endl;			//prints out support value
			// stationary support is the last to be computed, hence it is store_support_magnitude[1] instead.
	}


	void sortArray(){
		ofstream Shear_force("shear_force.csv");
		vector <pair <double, double> > vectPair;			//sorting purposes to couple force and location vector
		vector <double> shear_force;
		vector <double> plot_location;
		for (int i = 0; i < store_force.size(); i++) {			//sorting
			vectPair.push_back(make_pair(store_location[i], store_force[i]));		//sorting
		}
		
		sort(vectPair.begin(), vectPair.end());					//sorting the vectors. SORTS LOCATION IN ACSESDING ORDER.
		shear_force.push_back(vectPair[0].second);			//initialize 0th element of shear force with the initial force to the left. for plotting. 

		for (int i = 1; i < store_force.size(); i++) {						//loop through each force points. 0th element can be ignored sinc it is initialized
			shear_force.push_back(vectPair[i].second + vectPair[i - 1].second);  //subtract each force point with previous array. COMPUTES THE SHEAR FORCE FOR THE DIAGRAM.
		}

		for (int i = 0; i < store_force.size(); i++) {
			cout << vectPair[i].first << " " << vectPair[i].second << endl;	//vectPair first stores LOCATION. vectPair second stores the SHEAR FORCE.
		}
		 
		
		Shear_force << vectPair[0].first << " , " << 0 << endl;							//initialize ponit with zero.
		for (int force_plot = 0; force_plot < vectPair.size() - 1; force_plot++) {		//goes through each location input by user	
			for (double i = vectPair[force_plot].first; i < vectPair[force_plot + 1].first; i += 0.2) {	//goes through each location of array wih the increments of 0.2 for local
				if (i == vectPair[force_plot].first && force_plot != 0) {				//if the value of i is the value of the force_plot array
				cout << i << "   " << shear_force[force_plot - 1] << endl;
				Shear_force << i << "," << shear_force[force_plot - 1] << endl;	//plots the extra point two enable two points on t=one location.
				}
			cout << i << "   " << shear_force[force_plot] << endl;				//prints out each force in 0.2 increments instead
			Shear_force << i << "," << shear_force[force_plot] << endl;				//end the shear force with 0.
			}
		}
		Shear_force << vectPair[vectPair.size() - 1].first << "," << 0 << endl;			//end point with 0
		Shear_force.close();
	}


};


int main() {
	beamCalc beam;
	beam.setBeam();
	beam.setForces();
	beam.setSupports();
	beam.initBeam();
	beam.Compute_support_reaction();
	beam.sortArray();
	
	
	system("pause");
}
