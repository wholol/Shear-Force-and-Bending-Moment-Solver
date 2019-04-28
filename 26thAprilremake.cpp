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

class setBeamParams {		//this class will be reponsibile for: drawing the beam, setting up forces
protected:
	double beamlength;			//input parameter for the beam's length for form user
	char beamDraw[25][200];		//2D array for drawing the beam
	double scale_factor = 5;	//responsible for drawing the beam

	vector <double> store_forces;	//responsible for storing force values.
	vector <double> store_location;	//respornsible for storing location of forces. NEEDED FOR SECTIONING.
	vector <double> load_force_location_begin;	//stores the location of which the load force begins.
	vector <double> load_force_location_end;	//stores the location of which the load force ends.

public:
	void initBeam() {	//this member fucntion prints the beam in the console
		for (int row = 0; row < 25; row++) {
			for (int column = 0; column < 200; column++) {
				cout << beamDraw[row][column];
			}
			cout << endl;
		}
	}

	void setBeam() {		//this  member fucntion asks the user for the beamlength, and draws the beam (but not print it).
		for (int row = 0; row < 25; row++) {
			for (int column = 0; column < 200; column++) {
				beamDraw[row][column] = ' ';
			}
		}
		cout << "enter beam length. Software supprots a maximum of 38 m only." << endl;
		cin >> beamlength;
		while (beamlength > 38) {
			cout << "Value for than 38 m. Enter again." << endl;
			cin >> beamlength;
		}
		for (int column = 0; column <= beamlength * scale_factor; column++) {	//this function draws the beam.
			beamDraw[12][column] = '=';
		}
	}

	void setForces() {				//this function is for force set up. It look continuosly loop until all forces have een set up.
		char force_type; double force_location; char force_direction; double force_magnitude;
		double load_location_begin; double load_location_end;			//distributed loading force location. (begin and end).
		cout << "Enter p for point load, l for uniform loading" << endl;
		cin >> force_type;								//speificy force type
		while (force_type != 'p' && force_type != 'l') {
			cout << "Enter a valid force type." << endl;
			cin >> force_type;
		}

		if (force_type == 'p') {		// ***********POINT LOAD***************
			cout << "Enter force location." << endl;
			cin >> force_location;		// enter the force location
			while (force_location > beamlength) {
				cout << "force location exceeds the beamlength" << endl;
				cin >> force_location;
			}
			store_location.push_back(force_location);		//stores the location of the force.

			cout << "enter force direction. enter u for up, d for down." << endl;
			cin >> force_direction;
			while (force_direction != 'u' && force_direction != 'd') {
				cout << "invalid direction." << endl;
				cin >> force_direction;
			}

			if (force_direction == 'u') {					// if the force is upwards
				cout << "Enter force value. Only input the absolute value" << endl;
				cin >> force_magnitude;
				store_forces.push_back(force_magnitude);	//stores the force magnitude
			}

			if (force_direction == 'd') {		//if the force is downwards
				cout << "Enter the force value. Only input the absolute value" << endl;
				cin >> force_magnitude;
				store_forces.push_back(force_magnitude);	// stores the force value in the store forces container.
			}
		}

		if (force_type == 'l') {			//**************LOAD FORCE*********
			cout << "enter the force where it begins." << endl;
			cin >> load_location_begin;
			load_force_location_begin.push_back(load_location_begin);	//inserts the beginning location into the location begin vector.
			cout << "enter the force where it ends" << endl;		
			cin >> load_location_end;		
			load_force_location_end.push_back(load_location_end);		// inserts the end location into the location end vector.
			

			cout << "enter force direction. enter u for up, d for down." << endl;
			cin >> force_direction;
			while (force_direction != 'u' && force_direction != 'd') {
				cout << "invalid direction." << endl;
				cin >> force_direction;
			}

			if (force_direction == 'u') {					// if the force is upwards
				cout << "Enter force value. Only input the absolute value" << endl;
				cin >> force_magnitude;
				store_forces.push_back(force_magnitude);	//stores the force magnitude
			}

			if (force_direction == 'd') {		//if the force is downwards
				cout << "Enter the force value. Only input the absolute value" << endl;
				cin >> force_magnitude;
				store_forces.push_back(force_magnitude);	// stores the force value in the store forces container.
			}
		}
	}

	


};




int main() {
	setBeamParams beam;		//initialize class object
	beam.setBeam();			// sets the beam length
	beam.setForces();		// setup forces

	system("pause");
}
