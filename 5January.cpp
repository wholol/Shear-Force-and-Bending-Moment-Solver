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
ofstream Shear_force("shear_force.csv"); //instantiate object for writing data to csv file
ofstream bendingmoments("bending_moments.csv"); //instantiate object for beding moments csv file


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

///////////////////////////////////////////////////////////////////////////////////distributed force below//////////////////////////////////////////////////////////////////////////////
			
			if (force_type == 'l') {							//distributed load setting//
				double force_location_begin; double force_location_end; char force_direction_distributed;
				cout << " specify force location at left end (meters)." << endl;
				cin >> force_location_begin;								//force begin = left end. hence it should have a smaller value.
				store_load_force_location_begin.push_back(force_location_begin);
				

				while (force_location_begin > beamlength) {									// if location is greater than beamlength
					cout << "invalid output" << endl;
					cin >> force_location_begin;											// stores the location where the force begins
					store_load_force_location_begin.push_back(force_location_begin);		//stores the location where the force begins
				}

				cout << "specify force location at right end (meters)." << endl;	
				cin >> force_location_end;												
				store_load_force_location_end.push_back(force_location_end);				// stores the location where the force ends
				while (force_location_end > beamlength) {
					cout << "invalid output" << endl;
					store_load_force_location_end.push_back(force_location_end);			//stores the locaction where the force end
					cin >> force_location_end;
				}

				cout << "forces direction. enter u for upwards, d for downwards." << endl;
				cin >> force_direction_distributed;



				if (force_direction_distributed == 'u') {		//upward force for distributed load
					cout << "enter load distribution in N/m" << endl;
					cin >> force_value;
					store_load_force_magnitude_per_meter.push_back(force_value);		//stores load distribution gradient value. ideally same size as force begin and end
					
					for (int row = 14; row <= 16; row++) {											//this loop is pretty much for drawing purposes
						int ConvertForceBegin = force_location_begin * scale_factor;				//scale the force for drawing purposes and also convert to int
						int ConvertForceEnd = force_location_end * scale_factor;					// scale the force for drawing purposes and convert to int
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
		double support_location; char escape; int InitSupport = 9999;			//initiate the supprot value to be zero.
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
				if (store_support_location[0] > store_support_location[1]) {		//this condition allows us to set the smaller support location value to be always the first one.
					swap(store_support_location[0], store_support_location[1]);		//swaps supports. we always use the left support to initiate the moment calculation.
				}
				break;
			}
		}
	}
};

class beamCalc:public BeamProperties{
private:
	vector <double> shear_to_moments;
	vector <double> bending_moment_location;
	vector <double> shear_force_area;					//computes the shear force area to be added for the bending momenets
	vector <double> bending_moment_magnitude;
	vector <pair <double, double> > vectPair;			//sorting purposes to couple force and location vector
	vector <double> shear_force;						//shear force vector
	
public:


	void Compute_support_reaction() {
		double sum_moments_point_force = 0;				
		double sum_magnitude_point_force = 0;
		double sum_moments_load_force = 0;
		double sum_magnitude_load_force = 0;

		if (store_point_force_location.size() > 0) {					//checks for point force presence.
			for (int i = 0; i < store_point_force_location.size() ; i++) {		//loop through all point forces
				double pointForceMoments = store_point_force_magnitude[i] * (store_point_force_location[i] - store_support_location[0]);	//compute the moments for each point
				store_moments_point_force.push_back(pointForceMoments);				//contains the moments for each point force into this vector
				sum_moments_point_force += store_moments_point_force[i];			//sums all the moments of eahc force
				sum_magnitude_point_force += store_point_force_magnitude[i];		//sums all the magnitude of forces 
			}
		}

		if (store_load_force_location_begin.size() > 0) {												//checks for load force presence.
			for (int i = 0; i < store_load_force_magnitude_per_meter.size(); i++) {						//for each looad force element
				double storeCentroid = (store_load_force_location_end[i] - store_load_force_location_begin[i])*0.5;		//store the centroid value for each load
				//cout << store_load_force_location_begin[i] << "," << store_load_force_location_end[i] << endl;
				store_load_force_location_centroid.push_back(storeCentroid);// stores the centroid of each element into this array
				//cout << store_load_force_location_centroid[i] << endl;
				//cout << store_load_force_location_begin[i] << endl;
				//cout << store_support_location[0] << endl;
				
				
				double storeLoadForce = store_load_force_magnitude_per_meter[i] * (abs(store_load_force_location_begin[i] - store_load_force_location_end[i]));	//force*distance for force value.location not calculated.												
				store_load_force_magnitude.push_back(storeLoadForce);			//stores each load force into this vector
				//cout << storeLoadForce << endl;
				
				double storeLoadMoments = store_load_force_magnitude[i] * (store_load_force_location_centroid[i] + store_load_force_location_begin[i] - store_support_location[0]);		//store the load force moments
				store_moments_load_force.push_back(storeLoadMoments);		//stores all moments of each load forces into this array
				//cout << storeLoadMoments << endl;							
				
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
				if (store_force[i] == 9999 && store_location[i] == store_support_location[1]) {
					store_force[i] = store_support_magnitude[0];
				}
				else if (store_force[i] == 9999 && store_location[i] == store_support_location[0]) {
					store_force[i] = store_support_magnitude[1];
				}
					
			}
			cout << "the support at " << store_support_location[0] << " meters is " << store_support_magnitude[1] << endl;			//prints out support value
			// stationary support is the last to be computed, hence it is store_support_magnitude[1] instead.
	}


	void computeShearForce() {

		for (int i = 0; i < store_load_force_magnitude_per_meter.size(); i++) {				//for each load force available
			for (float j = store_load_force_location_begin[i]; j <= store_load_force_location_end[i]; j += 0.2) {		//further split the location into 0.2
				float shear_load = store_load_force_magnitude_per_meter[i] * (abs(store_load_force_location_begin[i] - j)); // computes the end point
				store_force.push_back(shear_load);								//stores shear load								
				store_location.push_back(j);									//stores the location of each point.

			}
		}

		for (int i = 0; i < store_force.size(); i++) {			//sorting the store_location vetor and store_force vector
			vectPair.push_back(make_pair(store_location[i], store_force[i]));		//sorting vector 

		}

		sort(vectPair.begin(), vectPair.end());					//sorting the vectors. SORTS LOCATION IN ACSESDING ORDER.
		shear_force.push_back(vectPair[0].second);			//initialize 0th element of shear force with the initial force to the left. for plotting.


		if (store_load_force_magnitude_per_meter.size() > 0) {	//if there are load forces persent in the diagram 						
			for (int i = 1; i < vectPair.size(); i++) {		//only initiate point load shear
				if (vectPair[i].first <= store_load_force_location_begin[0]) {	//perform constant shear subtraction for locations that are before the first load force
					//cout << vectPair[i].first << ",,,," << vectPair[i].second << endl;
					//cout << store_load_force_location_begin[0] << endl;
					shear_force.push_back(vectPair[i].second + shear_force[i - 1]);		//add each force point with previous array, since the force direction is already given.
				}
			}

			for (int k = 0; k < vectPair.size(); k++) {						//loops through each forces in the array
				if (vectPair[k].second == -0 || vectPair[k].second == 0) {					//if the load force detected is zero
					vectPair[k].second = shear_force[k - 1];				//set the zero force to the shear force before it.
					for (int j = 0; j < store_load_force_magnitude_per_meter.size(); j++) {			//goes through each possible load forces
						for (float z = store_load_force_location_begin[j]; z <= store_load_force_location_end[j]; z += 0.2) {	//loops through load force region
							float shear_load = vectPair[k].second + store_load_force_magnitude_per_meter[j] * (abs(store_load_force_location_begin[j] - z)); //linear shear subtraction
							cout << shear_load << "   test1   " << j << "  tetst2   " << z << endl;																										 //cout << shear_load << " , " << z << endl;				//displays the shear load 
							shear_force.push_back(shear_load);						//push backs the shear load into the shear_force vector
						}
					}
					//cout << vectPair[k].second <<  " ,,,," << vectPair[k].first << " ,,,,, " << shear_force[k -1] << endl;
				}
			}
		}

		else {										//if theres is only point forcesfor the beam
			for (int i = 1; i < store_force.size(); i++) {
				shear_force.push_back(vectPair[i].second + shear_force[i - 1]);
			}
		}

		for (double i = 0; i <= vectPair[0].first; i += 0.2) {
			Shear_force << i << "," << 0 << endl;				//fill the beam with zero forces, IF there is no shear force prevalent, or it is out of bounds
		}

		for (int force_plot = 0; force_plot < shear_force.size() - 1; force_plot++) {		//goes through each loaction, except the last one (last element)
			cout << vectPair[force_plot].first << "hhhhhh" << endl;
			for (float i = vectPair[force_plot].first; i < vectPair[force_plot + 1].first; i += 0.2) {	//further split the locations into increments of 0.2

				if (store_load_force_magnitude_per_meter.size() > 0) {			///////////////////////////load force presence////////////////////////////
					//cout << i << "," << shear_force[force_plot + 1] << endl;
					for (int p = 0; p < store_point_force_location.size(); p++) {		//checks if there is a presence of point force to plto etra point
						if (i == store_point_force_location[p]) {
							//cout << i << "," << shear_force[force_plot - 1] << endl;
							Shear_force << i << "," << shear_force[force_plot - 1] << endl;
						}
					}
					Shear_force << i << "," << shear_force[force_plot] << endl;		//end the shear force with 0

				}

				else {
					if (i == vectPair[force_plot].first && force_plot != 0) {				//if the value of i is the value of the location array, and it is not at the beginning.	
						Shear_force << i << "," << shear_force[force_plot - 1] << endl;		//plots the extra point on the location where the force exists.
						//cout << i << "," << shear_force[force_plot - 1] << endl;
					}
					Shear_force << i << "," << shear_force[force_plot] << endl;			//only for point force
					shear_to_moments.push_back(shear_force[force_plot]);		//to plot the bending moment of the diagram
												
				}
			}
		}

		for (double i = vectPair[vectPair.size() - 1].first; i <= beamlength; i += 0.2) {		//fills remaning parts of the beam that is out of bounds with zero
			Shear_force << i << "," << 0 << endl;
		}


		for (int i = 0; i < shear_force.size(); i++) {				//test//
			cout << "shear forces: " << shear_force[i] <<  endl;
		}

		for (int i = 0; i < vectPair.size(); i++) {
			cout << " these are the store_location numbers: " << vectPair[i].first << "   These are the store_force numbers: " << vectPair[i].second << endl;
		}
	}

	void computeBendingMoments() {
		bendingmoments << "beam distance" << "," << "bending moment" << endl;

		for (double i = 0; i <= vectPair[0].first; i += 0.2) {
			bendingmoments << i << "," << 0 << endl;				//fill the beam with zero forces, IF there is no shear force prevalent, or it is out of bounds
		}
		
		if (store_load_force_magnitude.size() > 0) {						// IF THERE IS A PRESENCE OF A DISTRIBUTED LOAD

	}	

		

		else {																//IF THERE IS ONLY POINT FORCES AVAILABLE

				for (int j = 0; j < shear_force.size() - 1; j++) {	//location loop
					double location = (vectPair[j + 1].first - vectPair[j].first);
					double shearForceArea = shear_force[j] * location;
					cout << shearForceArea << "these are the shear force areas " << endl;
					shear_force_area.push_back(shearForceArea);						//stores the area occupied by each shear force segment
				}

			//the zeroth shear force area is the same as the bending moments, hence we can initialize the first shear force area
			bendingmoments << vectPair[1].first << " , " << shear_force_area[0] << endl;	//initialize the first bending moment of the graph (the non zero bending moment).
			double moment = shear_force_area[0];		//initialize the bending moment to be the first shear force area
			bending_moment_magnitude.push_back(shear_force_area[0]);			//initialize the second bending moment of the graph and store that magnitude
			
			for (int i = 1; i < shear_force_area.size() - 1; i++){				//goes through each shear force area
				double moments_new = shear_force_area[i] + moment;
				bending_moment_magnitude.push_back(moments_new);
				moment = moments_new;
			}

			for (int i = 0; i < bending_moment_magnitude.size(); i++) {											//passes data to the csv file
				bendingmoments << vectPair[i + 1].first << "," << bending_moment_magnitude[i] << endl;			//data points
			}

			for (double i = vectPair[vectPair.size() - 1].first; i <= beamlength; i += 0.2) {		//fills remaning parts of the beam that is out of bounds with zero
				bendingmoments << i << "," << 0 << endl;
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
	beam.computeShearForce();
	beam.computeBendingMoments();
	
	
	system("pause");
}
