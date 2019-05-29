#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <fstream>

using namespace std;

ofstream parsedata("shear_force and bending moments.csv"); //instantiate object for writing data to csv file

class beamProperties {

protected:
	double beamlength; int PointLoadNumber; double PointLoadMagnitude; double PointLoadLocation; double SupportLocation;
	vector <double> PointLoadLocationVect; vector <double> PointLoadMagnitudeVect;		//vectors for point load magnitude and location.
	double SupportLocationArray[2];

	int UDLnumber; double UDLmagnitude; double UDLbegin; double UDLend;
	vector <double> UDLlocation_beginVect; vector <double> UDLlocation_endVect; vector <double> UDLmagnitudeVect;

public:

	void initbeam() {
		cout << "Welcome to the shear force and bending moment solver. Enter beam length to get started." << endl;
		cin >> beamlength;
	}

	void PointLoadSetup() {
		cout << "Enter number of point loads that you have." << endl;
		cin >> PointLoadNumber;
		cout << "   " << endl;
		if (PointLoadNumber > 0) {
		cout << "Enter Point Load magnitude, followed by its location." << endl;
			for (int i = 0; i < PointLoadNumber; ++i) {
				cout << "   " << endl;
				cout << "enter point load magnitude." << endl;
				cin >> PointLoadMagnitude;
				PointLoadMagnitudeVect.push_back(PointLoadMagnitude);
				cout << "   " << endl;
				cout << "Enter Point Load location" << endl;
				cin >> PointLoadLocation;
				PointLoadLocationVect.push_back(PointLoadLocation);
			}
		}
	}

	void UDLSetup() {		//uniform distributed loading setup.
		cout << " enter number of distributed loads." << endl;
		cin >> UDLnumber;
		for (int i = 0; i < UDLnumber; i++) {
			cout << "Enter distributed loading w value, followed by its beginninglocation, lastly the end location. (start from left)." << endl;
			cin >> UDLmagnitude;
			UDLmagnitudeVect.push_back(UDLmagnitude);
			cout << " " << endl;
			cout << "enter beginning of location (from left)." << endl;
			cin >> UDLbegin;
			UDLlocation_beginVect.push_back(UDLbegin);
			cout << " " << endl;
			cout << " enter ed of location (from right)" << endl;
			cin >> UDLend;
			UDLlocation_endVect.push_back(UDLend);

		}
	}

	void Supportsetup() {			//initialize the [0]th element to the initial beam length, and [1]st element to the value of the beamlength
		SupportLocationArray[0] = 0;
		SupportLocationArray[1] = beamlength;
	}

	int getPointLoadNumber() const {
		return PointLoadNumber;
	}

	int getUDLNumber() const {
		return UDLnumber;
	}
};


class ComputeSupports :public beamProperties {
protected:
	vector <double> LeftSupports; vector <double> RightSupports;		//these vectors are for the superposition vectors for right and left supports.
	vector <double> UDLLeftSupports; vector <double> UDLRightSupports;
	
public:

	void PointLoadSupports() {			//determine the support value for each instance.
		for (int i = 0; i < PointLoadNumber; i++) {
			RightSupports.push_back((-(PointLoadLocationVect[i] - SupportLocationArray[0])*PointLoadMagnitudeVect[i])/(SupportLocationArray[1] - SupportLocationArray[0]));	//calcualte each instance of the superposition left support
			LeftSupports.push_back(((PointLoadLocationVect[i] - SupportLocationArray[1])*PointLoadMagnitudeVect[i])/(SupportLocationArray[1] - SupportLocationArray[0]));	//calculates each instance of the superposition right support
		}

		for (int i = 0; i < LeftSupports.size(); i++) {
			cout << LeftSupports[i] << " left is left, rght is (for point load) " << RightSupports[i] << endl;
		}
	}

	void UDLSupports() {			//determine the support values for UDL.
		
			for (int j = 0; j < UDLnumber; j++) {								//this loop performs the computation for the supports.
				UDLLeftSupports.push_back((-(UDLlocation_endVect[j] - UDLlocation_beginVect[j])*UDLmagnitudeVect[j])*(SupportLocationArray[1] - (0.5*(UDLlocation_endVect[j] + UDLlocation_beginVect[j])))/SupportLocationArray[1]);
				UDLRightSupports.push_back(-(UDLlocation_endVect[j] - UDLlocation_beginVect[j])*(UDLmagnitudeVect[j])*(0.5*(UDLlocation_endVect[j] + UDLlocation_beginVect[j]))/SupportLocationArray[1]);
			}

			for (int i = 0; i < UDLLeftSupports.size(); i++) {
				cout << UDLLeftSupports[i] << " left is left, rght is (for UDL load) " << UDLRightSupports[i] << endl;
			}
	}

};

class ShearForces :public ComputeSupports {
protected:
	vector <double> PointForceVal;			//stores all point force values, including support reactions.
	vector <double> PointForceLocal;		//stores all point force locations, including support values.
	vector <pair <double, double>> PointLoadPair;					//pairs the pint load magnitude and location.
	vector <pair <double, double>> PointShearFinalPair;					//pairs the final shear force value and location for point load.
	vector <double> PointShearVect;				//stores the shear forces.

	vector <double> ShearLocationSuperposition;		// this location vector stores the location for shear for each superposition
	vector <double> PointShearSuperposition;		// this value stores the values for the point load shears for each superposition.
	vector <double> ShearLocationFinal;				//final plots after superpostion
	vector <double> PointShearFinal;				//final shear values.
	vector <double> removeShear;					//remove redundant shear laods by the point load
	int element = 0;
	int storeLastLocation;							//stores the last element of the shear lcoation before adding zeros.


	vector <double> UDLForceVal;	//includes all force values, including
	vector <double> UDLForceLocal;
	vector <double> UDLSuperpositionLocation;
	vector <double> UDLSuperpositionShear;
	vector <double> UDLShearFinal;
	vector <double> UDLLocationFinal;

public:
	void PointLoadShearSetup() {			//this function computes each instance of the point load shear
		for (int i = 0; i < PointLoadNumber; i++) {
			PointForceVal.push_back(RightSupports[i]);					//pushes back the right support value
			PointForceVal.push_back(LeftSupports[i]);					//pushes back the left support value
			PointForceVal.push_back(PointLoadMagnitudeVect[i]);			//pushes back the point load magnitude vector
			PointForceLocal.push_back(SupportLocationArray[1]);
			PointForceLocal.push_back(SupportLocationArray[0]);
			PointForceLocal.push_back(PointLoadLocationVect[i]);
		}

		for (int i = 0; i < PointForceLocal.size(); i++) {								//sorting the store_location vetor and store_force vector
			PointLoadPair.push_back(make_pair(PointForceLocal[i], PointForceVal[i]));	//sorts location vector.
		}

		/*for (int i = 0; i < PointLoadPair.size(); i++) {
			cout << PointLoadPair[i].first << " <- local force-> " << PointLoadPair[i].second << endl;
		}*/

		for (int i = 0; i < PointLoadPair.size(); i += 3) {								// sorts the location of point forces according to each force element.
			if (PointLoadPair[i].first > PointLoadPair[i + 1].first) {
				swap(PointLoadPair[i].first, PointLoadPair[i + 1].first);
				swap(PointLoadPair[i].second, PointLoadPair[i + 1].second);
			}

			if (PointLoadPair[i + 1].first > PointLoadPair[i + 2].first) {
				swap(PointLoadPair[i + 2].first, PointLoadPair[i + 1].first);
				swap(PointLoadPair[i + 2].second, PointLoadPair[i + 1].second);
			}

			if (PointLoadPair[i].first > PointLoadPair[i + 1].first) {
				swap(PointLoadPair[i].first, PointLoadPair[i + 1].first);
				swap(PointLoadPair[i].second, PointLoadPair[i + 1].second);
			}
		}

		for (int i = 0; i < PointLoadPair.size(); i++) {
			//cout << "force number  " << i << endl;
			cout << PointLoadPair[i].first << " <- location of forces/supports || local force-> " << PointLoadPair[i].second << endl;

		}
	}

	void ComputePointShear() {//computes the shear forces for the point load forces.

		for (int i = 0; i < PointLoadPair.size(); i += 3) {
			PointShearVect.push_back(PointLoadPair[i].second);
			PointShearVect.push_back(PointLoadPair[i].second + PointLoadPair[i + 1].second);
		}

		//for (int i = 0; i < PointShearVect.size(); i++) {			//this loop computes the local shear forces. take note that each superpostion will only have two shear forces (since each superposition will have two supports + one poit force only.
		//	cout << PointShearVect[i] << "<-- ShearForces " << PointLoadPair[i + 1].first << endl;	//critical points at which the forces change values.	
		//}

		int shearsize = 0;			//initialize varialbe called shear size
		int increment = 0;			//checks the element of pointloadpair
		bool loop = false;
		double checkLastLocation = 2;

		while (1) {			//this while loop produces the
			for (int h = 3; h < PointLoadPair.size(); h += 3) {
				if (increment == h - 1) {		//since we dont want to compute from the last element of the first force to the beginning of the second force.
					increment = h;				//immediately set the increment to the first force.
					loop = true;
					checkLastLocation = h - 1;

				}
			}

			if (loop == true) {
				double checkLastLocation = increment;
			}
			//cout << j << " " << shearsize << endl;

			for (double i = 0; i < PointLoadPair.size(); i += 3) {	//checks the initial length.
				if (increment == i && PointLoadPair[i].first != 0) {
					for (double j = 0; j <= PointLoadPair[i].first * 10; j++) {
						PointShearSuperposition.push_back(0);
						ShearLocationSuperposition.push_back(j / 10);
					}
				}
			}

			for (double k = (PointLoadPair[increment].first) * 10; k <= (PointLoadPair[increment + 1].first) * 10; k++) {		//for each point load pair location.
				ShearLocationSuperposition.push_back(k / 10);											//push back the location distance, incremented by 0.2
				PointShearSuperposition.push_back(PointShearVect[shearsize]);							//push back the shear force value.

				for (int i = 1; i < PointLoadPair.size(); i += 3) {										//inserts extra shear values due at the transition region of each individual superpostion forces.
					if (k == PointLoadPair[i].first * 10 && k != PointLoadPair[checkLastLocation].first) {				//if k is a transition location but also not the last location.			
						ShearLocationSuperposition.push_back(k / 10);
						PointShearSuperposition.push_back(PointShearVect[shearsize]);
					}
				}
			}

			/*for (double i = 2; i < PointLoadPair.size(); i += 3) {					//loop to insert zeroes in the last length.
				if ((increment + 1) == i && PointLoadPair[increment + 1].first <= beamlength) {
					for (double j = PointLoadPair[increment + 1].first * 10; j <= beamlength * 10; j++) {
						/*for (int k = 2; k < PointLoadPair.size(); k += 3) {			//checks if there are other trnasitonal regions after the maximum location.
							if (j == PointLoadPair[k].first*10) {
								PointShearSuperposition.push_back(0);
								ShearLocationSuperposition.push_back(j / 10);
							}
						}
						PointShearSuperposition.push_back(0);
						ShearLocationSuperposition.push_back(j / 10);

					}
				}
			}*/

			increment = increment + 1;
			shearsize += 1;
			if (shearsize >= PointShearVect.size()) {
				break;
			}

		}
		for (int i = 1; i < ShearLocationSuperposition.size() - 1; i++) {
			if ((PointShearSuperposition[i - 1] != PointShearSuperposition[i]) && (ShearLocationSuperposition[i] == ShearLocationSuperposition[i - 1])) {	//if the force value is different and the location is the same, provided that the location is not the first or lat location.
				//cout << PointShearSuperposition[i - 1] << " chosen values" << PointShearSuperposition[i] << " chosen values"  << ShearLocationSuperposition[i] << " chosen values"  << ShearLocationSuperposition[i - 1] << endl;
				for (int j = 1; j < PointLoadPair.size(); j += 3) {
					if ((ShearLocationSuperposition[i] == PointLoadPair[j].first) && (ShearLocationSuperposition[i - 1] == PointLoadPair[j].first)) { //if the loads are not the beginning and end loads
						cout << PointLoadPair[j].first << endl;
						//cout << PointShearSuperposition[i - 1] << " delete these " << PointShearSuperposition[i] << " delete these " << ShearLocationSuperposition[i] << " delete these " << ShearLocationSuperposition[i - 1] << endl;
						removeShear.push_back(i);
					}
				}
				//cout << PointShearSuperposition[i - 1] << " " << PointShearSuperposition[i] << " " << ShearLocationSuperposition[i] << " " << ShearLocationSuperposition[i - 1] << endl;
			}
		}

		for (int j = 0; j < removeShear.size(); j++) {		//thi loop is rsponsible fo removing assitional shear forces caused by the addtion of shear forces.
			PointShearSuperposition.erase(PointShearSuperposition.begin() + removeShear[j] - element);
			PointShearSuperposition.erase(PointShearSuperposition.begin() + removeShear[j] - 1 - element);
			ShearLocationSuperposition.erase(ShearLocationSuperposition.begin() + removeShear[j] - element);
			ShearLocationSuperposition.erase(ShearLocationSuperposition.begin() + removeShear[j] - 1 - element);
			element += 2;			//elements is needed. because the first two shears are removed, the element is decreased by 2.
		}

		for (int i = 0; i < ShearLocationSuperposition.size(); i++) {
			cout << ShearLocationSuperposition[i] << " <--- Location Plot || Shear value Superposition --> " << PointShearSuperposition[i] << endl;

		}

		const int SuperPositionSize = ShearLocationSuperposition.size();

		for (int i = 0; i <= (ShearLocationSuperposition.size() / PointLoadNumber) - 1; i++) {	//pushes back the first force values into the final shear force array
			PointShearFinal.push_back(PointShearSuperposition[i]);
		}

		for (int i = 0; i < PointShearFinal.size(); i++) {
			ShearLocationFinal.push_back(ShearLocationSuperposition[i]);
		}

		storeLastLocation = ShearLocationFinal[PointShearFinal.size() - 1];		//stores the last lcoation of the shear force (this variable is used to add zeroes if the last lcoation != beamlength.

		if (PointLoadNumber != 1) {					//if there is more than one point load, the superposition vector is split into parts and added to each index.
			for (int k = 1; k < PointLoadNumber; k++) {		//for each each instance of point load.
				for (int i = 0; i <= (ShearLocationSuperposition.size() / PointLoadNumber) - 1; i++) { //this loop sums all the point loads together.
					PointShearFinal[i] += PointShearSuperposition[k*(ShearLocationSuperposition.size() / PointLoadNumber) + i];	//sums the superposition forces together
				}
			}
		}

		/*if (ShearLocationFinal[0] != 0) {										//if the initial location is not zero, fill the beam with zero values and forces.
			for (double i = 0*10; i <= ShearLocationFinal[0]*10; i++) {

				ShearLocationFinal.push_back(i / 10);
				PointShearFinal.push_back(0);
			}
		}

		if (storeLastLocation <= beamlength) {		// if the final locationis not the beamlength.
			cout << storeLastLocation << endl;
			for (double i = storeLastLocation*10 + 2; i <= beamlength*10; i++) {
				ShearLocationFinal.push_back(i / 10);
				PointShearFinal.push_back(0);

			}
			ShearLocationFinal.push_back(storeLastLocation + 0.000001);		//since the sorting is unstable.
			PointShearFinal.push_back(0);
		}*/

		for (int i = 0; i < PointShearFinal.size(); i++) {			//create a pair vector for the final shear location vector and the final shear force vector
			PointShearFinalPair.push_back(make_pair(ShearLocationFinal[i], PointShearFinal[i]));
		}

		for (int i = 0; i < PointShearFinalPair.size() - 1; i++) {			//bubble sort to sort the vector,since the sort function in an unstable algo.
			for (int j = 0; j < PointShearFinalPair.size() - 1; j++) {
				if (PointShearFinalPair[j].first > PointShearFinalPair[j + 1].first) {		//if the previous element is greater than the later element.
					double temp = PointShearFinalPair[j + 1].first;
					PointShearFinalPair[j + 1].first = PointShearFinalPair[j].first;
					PointShearFinalPair[j].first = temp;
				}
			}
		}

		for (int i = 0; i < PointShearFinalPair.size(); i++) {
			cout << PointShearFinalPair[i].first << "<----LOCATION || SHEAR VALUE FINAL ---->" << PointShearFinalPair[i].second << endl;
		}
	}

	void computeUDLShear() {

		if (PointLoadNumber > 0) {
			for (int i = 1; i <= UDLnumber; i++) {			//this loop pushes back the required locations for the UDL location for each instance, provided that there was a presence of a point force. (since point shears contain the transition region).
				for (int j = 0; j < PointShearFinalPair.size(); j++) {
					UDLSuperpositionLocation.push_back(PointShearFinalPair[j].first);
				}
			}
		}
		else {
			for (int i = 1; i <= UDLnumber; i++)
				for (double j = 0 * 10; j <= beamlength * 10; j++) {		//push back values of 0 to the beam's length at increments of 0.2
					UDLSuperpositionLocation.push_back(j / 10);
				}
		}

		for (int i = 0; i < UDLSuperpositionLocation.size(); i++) {			//prints out superpostion lcoation values.
			cout << UDLSuperpositionLocation[i] << endl;
		}

		for (int i = 0; i < UDLSuperpositionLocation.size(); i++) {			//intiialize superposition shear forces to zero.
			UDLSuperpositionShear.push_back(0);
		}

		for (int i = 0; i < UDLlocation_beginVect.size(); i++) {
			cout << UDLlocation_beginVect[i] << " <- begin" << UDLlocation_endVect[i] << "<- end" << UDLLeftSupports[i] << "<-- left supp" << UDLRightSupports[i] << endl;
		}
	
		int increment_location = 0, increment_size = (UDLSuperpositionLocation.size() / UDLnumber) - 1;
		int counter = 1;
			
				//for each instance of UDL load, compute each superposition shear
				cout << "test" << endl;
				cout << increment_location << endl;
				cout << increment_size << endl;
				cout << UDLSuperpositionLocation.size() << endl;
				for (int i = 0; i < UDLlocation_beginVect.size(); i++) {		//for each instance of UDL magnitude
					
						for (int j = increment_location; j <= increment_size; j++) {

							if (UDLSuperpositionLocation[j] <= UDLlocation_beginVect[i]) {		//while the location is smaller of equalto the the begining value of the UDL.
								cout << i << endl;
								UDLSuperpositionShear[j] = UDLLeftSupports[i];						//set the shear V be the left support reaction value
							}

							if ((UDLSuperpositionLocation[j] > UDLlocation_beginVect[i]) && (UDLSuperpositionLocation[j] < UDLlocation_endVect[i])) {	//if it is at the region of UDL
								UDLSuperpositionShear[j] = UDLLeftSupports[i] + UDLmagnitudeVect[i] * (UDLSuperpositionLocation[j] - UDLlocation_beginVect[i]);		// V = R + w(x - beginning location), where by w can be both postiive or negative.
							}

							if (UDLSuperpositionLocation[j] >= UDLlocation_endVect[i]) {		//after the UDL
								UDLSuperpositionShear[j] = -UDLRightSupports[i];				//V = -supportreaction
								
							}
							if (increment_location < (UDLSuperpositionLocation.size() - 1) && increment_size < (UDLSuperpositionLocation.size() - 1)) {	//prevent segfault
								increment_location += (UDLSuperpositionLocation.size() / UDLnumber);		//increment the initial superpositino lcoation
								increment_size += (UDLSuperpositionLocation.size() / UDLnumber);			//increment the the last element for each superposition
								cout << increment_location << endl;
								cout << increment_size << endl;
							}			
						}
					
				}
				
		for (int i = 0; i < UDLSuperpositionLocation.size(); i++) {
			cout << UDLSuperpositionLocation[i] << "<-- UDL SUPERPOSITION LOCATION || UDL SUPERPOSITION  SHEAR-->" << UDLSuperpositionShear[i] << endl;
		}

		for (int i = 0; i < (UDLSuperpositionLocation.size() / UDLnumber); i++) {		//this loop initializes the final location and shear values.
			UDLLocationFinal.push_back(UDLSuperpositionLocation[i]);
			UDLShearFinal.push_back(UDLSuperpositionShear[i]);
		}

		for (int k = 1; k < UDLnumber; k++) {			//this loop adds all the superposition values into a final value.
			for (int i = 0; i <= (UDLSuperpositionLocation.size() / UDLnumber) - 1; i++) {
				UDLShearFinal[i] += UDLSuperpositionShear[k*(UDLSuperpositionLocation.size() /UDLnumber) + i];
			}
		}

		for (int i = 0; i < UDLShearFinal.size(); i++) {			//prints out the 
			cout << UDLLocationFinal[i] << "<--- final Uniform distributed location|| final UDL shear --->" << UDLShearFinal[i] << endl;
		}
	}	

	void ParseShearData() {			//parses data to excel
		if (PointLoadNumber > 0 && UDLnumber == 0) {		//if there is only point load
			parsedata << "location" << "," << "Shear Force" << endl;
			parsedata << 0 << "," << 0 << endl;
			for (int i = 0; i < PointShearFinal.size(); i++) {
				parsedata << PointShearFinalPair[i].first << "," << PointShearFinalPair[i].second << endl;
			}
			parsedata << beamlength << "," << 0 << endl;
		}

		if (PointLoadNumber == 0 && UDLnumber > 0) {		//if there is only UDL 
			parsedata << "location" << "," << "Shear Force" << endl;
			parsedata << 0 << "," << 0 << endl;
			for (int i = 0; i < UDLShearFinal.size(); i++) {
				parsedata << UDLLocationFinal[i] << "," << UDLShearFinal[i] << endl;
			}
			parsedata << beamlength << "," << 0 << endl;
		}

		if (PointLoadNumber > 0 && UDLnumber > 0) {		//if both loads exists
			parsedata << "location" << "," << "Shear Force" << endl;
			parsedata << 0 << "," << 0 << endl;
			for (int i = 0; i < PointShearFinalPair.size(); i++) {
				parsedata << (UDLLocationFinal[i]) << "," << (PointShearFinalPair[i].second += UDLShearFinal[i]) << endl;
			}
			parsedata << beamlength << "," << 0 << endl;
		}
	}

};


class BendingMoments :public ShearForces {
protected:
	vector <double> PointLoadSuperPositionMoments;
	vector <double> PointMomentsFinal;

	vector <double> UDLSuperPositionMoments;
	vector <double> UDLMomentsFinal;
	
public:

	void ComputePointLoadMoments(){

		for (int i = 0; i < ShearLocationSuperposition.size(); i++) {		//intiialzie superpostion moments vector
			PointLoadSuperPositionMoments.push_back(0);
		}
		int counttransition = 1;
		int pointshearsuperpositionlocation = 0; int pointshear_size = (ShearLocationSuperposition.size() / PointLoadNumber) - 1;
		
		for (int z = 1; z < PointLoadPair.size(); z+=3){		//for each instance of point load
			for (int i = pointshearsuperpositionlocation; i < pointshear_size; i++) {			//for all superposition values
			
					if ((ShearLocationSuperposition[i] == ShearLocationSuperposition[i + 1]) && (PointShearSuperposition[i] != PointShearSuperposition[i + 1] && ShearLocationSuperposition[i] == PointLoadPair[z].first)) {	//is the location is of the same value but different force value.
						for (int k = 0; k <= i; k++) {
							PointLoadSuperPositionMoments[k] = (ShearLocationSuperposition[k] * PointShearSuperposition[k]);		//compute the moment of the first half, M = V*x
						}
						for (int j = i + 1; j < ShearLocationSuperposition.size(); j++) {						//compute the moment of the second half.
							//cout << ShearLocationSuperposition[j] << " " << PointShearSuperposition[j]<< endl;
							PointLoadSuperPositionMoments[j] = ((ShearLocationSuperposition[j] * PointShearSuperposition[j]) - (PointShearSuperposition[j] * beamlength));		// M = mx + c, m = shear force, c = (-V*beamlength).
							cout << PointLoadSuperPositionMoments[i + 1] << endl;
							cout << PointLoadSuperPositionMoments[51] << endl;
						}
						cout << PointShearSuperposition[i] << " <<egeg" << PointShearSuperposition[i + 1] << endl;
					}

				if (pointshearsuperpositionlocation < (ShearLocationSuperposition.size() - 1) && pointshear_size < (ShearLocationSuperposition.size() - 1)) {
					pointshearsuperpositionlocation += (ShearLocationSuperposition.size() / PointLoadNumber);
					pointshear_size += (ShearLocationSuperposition.size() / PointLoadNumber);
					
				}
			}
			
		}

		for (int i = 0; i < PointLoadSuperPositionMoments.size(); i++) {
			cout << ShearLocationSuperposition[i] << "<-- location superpostion || moment superposition -->" << PointLoadSuperPositionMoments[i] << endl;
			cout << PointLoadSuperPositionMoments[51] << endl;
 		}

		for (int i = 0; i <= (PointLoadSuperPositionMoments.size() / PointLoadNumber) - 1; i++) {	//pushes back the first moment values into the final moment
		PointMomentsFinal.push_back(PointLoadSuperPositionMoments[i]);
		}


		for (int k = 1; k < PointLoadNumber; k++) {		//for each each instance of point load.
			for (int i = 0; i <= (PointLoadSuperPositionMoments.size() / PointLoadNumber) - 1; i++) { //this loop sums all the point loads together.
				PointMomentsFinal[i] += PointLoadSuperPositionMoments[k*(ShearLocationSuperposition.size() / PointLoadNumber) + i];	//sums the superposition forces together
			}
		}

		for (int i = 0; i < PointMomentsFinal.size(); i++) {
			cout << PointShearFinalPair[i].first << " <-- FINAL LOCATION || FINAL MOMENTS --->" << PointMomentsFinal[i] << endl;
		}
	}

	void computeUDLMoments() {
		for (int i = 0; i < UDLSuperpositionLocation.size(); i++) {		//initialize the moments vector
			UDLSuperPositionMoments.push_back(0);
		}

		int increment_momentslocation = 0;
		int increment_momentssize = (UDLSuperpositionLocation.size() / UDLnumber) - 1;

		for (int i = 0; i < UDLlocation_beginVect.size(); i++) {//for each instance of UDL
			for (int j = increment_momentslocation; j <= increment_momentssize; j++) {
				if (UDLSuperpositionLocation[j] <= UDLlocation_beginVect[i]) {		//while the location is smaller of equalto the the begining value of the UDL.
					UDLSuperPositionMoments[j] = UDLLeftSupports[i] * UDLSuperpositionLocation[j];						//set the shear V be the left support reaction value
				}

				if ((UDLSuperpositionLocation[j] > UDLlocation_beginVect[i]) && (UDLSuperpositionLocation[j] < UDLlocation_endVect[i])) {	//if it is at the region of UDL
					UDLSuperPositionMoments[j] = (UDLLeftSupports[i] * UDLSuperpositionLocation[j]) + (UDLmagnitudeVect[i] * 0.5 * (UDLSuperpositionLocation[j] - UDLlocation_beginVect[i])*(UDLSuperpositionLocation[j] - UDLlocation_beginVect[i]));		// V = R + w(x - beginning location), where by w can be both postiive or negative.
				}

				if (UDLSuperpositionLocation[j] >= UDLlocation_endVect[i]) {		//after the UDL
					UDLSuperPositionMoments[j] = UDLRightSupports[i] * (beamlength - UDLSuperpositionLocation[j]);				//V = -supportreaction

				}
				if (increment_momentslocation < (UDLSuperpositionLocation.size() - 1) && increment_momentssize < (UDLSuperpositionLocation.size() - 1)) {	//prevent segfault
					increment_momentslocation += (UDLSuperpositionLocation.size() / UDLnumber);		//increment the initial superpositino lcoation
					increment_momentssize += (UDLSuperpositionLocation.size() / UDLnumber);			//increment the the last element for each superposition
					//cout << increment_momentslocation << endl;
					//cout << increment_momentssize << endl;
				}
			}
		}

		for (int i = 0; i < UDLSuperPositionMoments.size(); i++) {
			cout << UDLSuperpositionLocation[i] << "<-- SuperPositionLocation UDL || Superposition moments UDL -->" << UDLSuperPositionMoments[i] <<  endl;
		}

		for (int i = 0; i < (UDLSuperPositionMoments.size() / UDLnumber); i++) {		//this loop initializes the final location and shear values.
			UDLMomentsFinal.push_back(UDLSuperPositionMoments[i]);
		}

		for (int k = 1; k < UDLnumber; k++) {			//this loop adds all the superposition values into a final value.
			for (int i = 0; i <= (UDLSuperPositionMoments.size() / UDLnumber) - 1; i++) {
				UDLMomentsFinal[i] += UDLSuperPositionMoments[k*(UDLSuperPositionMoments.size() / UDLnumber) + i];
			}
		}
	}

	
	void ParseMomentsData() {
		if (PointLoadNumber > 0 && UDLnumber == 0) {		//if there is only point load
			parsedata << "," << "," << "," << "location" << "," << "bending moments" << endl;
			for (int i = 0; i < PointShearFinal.size(); i++) {
				parsedata << "," << "," << "," << PointShearFinalPair[i].first << "," << PointMomentsFinal[i] << endl;
			}
		}

		if (PointLoadNumber == 0 && UDLnumber > 0) {		//if there is only UDL 
			parsedata << "," << "," << "," << "location" << "," << "bending moments" << endl;
			for (int i = 0; i < UDLShearFinal.size(); i++) {
				parsedata << "," << "," << "," << UDLLocationFinal[i] << "," << UDLMomentsFinal[i] << endl;
			}
		}

		if (PointLoadNumber > 0 && UDLnumber > 0) {		//if both loads exists
			parsedata << "," << "," << "," << "location" << "," << "bending moments" << endl;
			for (int i = 0; i < UDLLocationFinal.size(); i++) {
				parsedata <<"," << "," << "," << (UDLLocationFinal[i]) << "," << (PointMomentsFinal[i] += UDLMomentsFinal[i]) << endl;
			}
		}
	}
};

int main() {

	BendingMoments beam;
	beam.initbeam();
	beam.PointLoadSetup();
	beam.UDLSetup();
	beam.Supportsetup();
	

	if (beam.getPointLoadNumber() != 0) {
		beam.PointLoadSupports();
		beam.PointLoadShearSetup();
		beam.ComputePointShear();
		beam.ComputePointLoadMoments();
	}

	if (beam.getUDLNumber() != 0) {
		beam.UDLSupports();
		beam.computeUDLShear();
		beam.computeUDLMoments();
	}

	if (beam.getUDLNumber() != 0 || beam.getPointLoadNumber() != 0) {
		beam.ParseShearData();
		beam.ParseMomentsData();
	 }
	system("pause");

}
