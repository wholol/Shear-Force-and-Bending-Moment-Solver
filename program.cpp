//Author:Der Zhi Jeremy Sow
//Date: 20/11/18

#include "pch.h"
#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <vector>
#include <algorithm>

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
			double force_location; char force_direction; char escape; char force_type; double force_magnitude; //beam properties for drawing
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
					cin >> force_magnitude;
					store_point_force_magnitude.push_back(force_magnitude*-1);			//negative force.
					for (int row = 10; row >= 4; row--) {
						int beamlengthConvert = force_location * scale_factor;
						beamDraw[row][beamlengthConvert] = '|';
						beamDraw[11][beamlengthConvert] = 'v';
					}
				}

				if (force_direction == 'u') {									//upward force
					cout << "enter force magnitude in Newton. (postive only)" << endl;
					cin >> force_magnitude;
					store_point_force_magnitude.push_back(force_magnitude);		//store force
					for (int row = 14; row <= 18; row++) {
						int beamlengthConvert = force_location * scale_factor;
						beamDraw[row][beamlengthConvert] = '|';
						beamDraw[13][beamlengthConvert] = '^';
					}
				}
			}

			////////distributed force below//////////
			if (force_type == 'l') {							//distributed load setting//
				double force_location_begin; double force_location_end; char force_direction_distributed;
				cout << " specify force one end (meters)." << endl;
				cin >> force_location_begin;

				while (force_location_begin > beamlength) {
					cout << "invalid output" << endl;
					cin >> force_location_begin;
				}

				cout << "specify other end of force (meters)." << endl;
				cin >> force_location_end;
				while (force_location_end > beamlength) {
					cout << "invalid output" << endl;
					cin >> force_location_begin;
				}

				cout << "forces direction. enter u for upwards, d for downwards." << endl;
				cin >> force_direction_distributed;

				if (force_direction_distributed == 'u') {		//upward force for distributed load
					cout << "enter load distribution in N/m" << endl;
					cin >> force_magnitude;
					store_load_force_magnitude_per_meter.push_back(force_magnitude);
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
					cin >> force_magnitude;
					store_load_force_magnitude_per_meter.push_back(force_magnitude*-1);
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
		double support_location; char escape;
		while (1) {
			cout << "enter your support locations." << endl;
			cin >> support_location;
			int supportConvert = support_location * scale_factor;
			while (beamDraw[13][supportConvert] == '^') {
				cout << "there is currently a force being placed here. Insert support again." << endl;
				cin >> support_location;
				supportConvert = support_location * scale_factor;				
			}
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
			cout << "the support at " << store_support_location[0] << " meters is " << store_support_magnitude[1] << endl;			//prints out support value
			// stationary support is the last to be cpmputed, hence it is store_support_magnitude[1] instead.
	}

	void ComputeShearForce() {
		if (store_load_force_magnitude.size() == 0){					//no presence of load force
			for (int i = 0; i < store_point_force_location.size(); i++) {
				if (store_point_force_location[i] > store_support_location[0] && store_point_force_location[i] > store_support_location[1]) {//if force is to the rightmost
					if (store_support_location[0] > store_support_location[1]) {			//if store 0 is to the right of store 1 for the support
						store_support_magnitude[1];		//initalize shear force with this value
						store_support_location[0];
						for (int i = 0; i < beamlength; i++) {
							for (int j = 0; j < store_point_force_location.size(); j++) {
								if (store_point_force_location[j] == beamlength) {
									//if (beamlength < )
								}
							}
							store_support_magnitude[1];
						}
					}										
					else {
						store_support_magnitude[0];			//initialize shear force with this support value
						store_support_location[1];
					}
				}

				if (store_point_force_location[i] < store_support_location[0] && store_point_force_location[i] < store_support_location[1]) {		//if force if to the leftmost
					store_load_force_magnitude[i];																	//display the force, to be plotted. at first.
				}

				

			}
			
		

		}
	} 


};


int main() {
	beamCalc beam;
	beam.setBeam();
	beam.setForces();
	beam.setSupports();
	beam.initBeam();
	beam.Compute_support_reaction();
	
	
	system("pause");
}
