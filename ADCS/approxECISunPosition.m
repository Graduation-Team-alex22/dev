function vector = approxECISunPosition(UTCTime)
% Given Gregorian year(s), day(s), and time(s), calculate Earth-centered
% inertial (ECI) coordinates (in meters) of the vector(s) pointing from
% Earth's center to the center of the Sun.
% Encapsulated by Seth B. Wagenman, from stargazing.net/kepler/sun.html as
% corrected by https://astronomy.stackexchange.com/a/37199/34646 along with
% other minor corrections based on recent revisions to the ecliptic plane.

% Set time(s) relative to the J2000.0 epoch
fractionalDay = juliandate(UTCTime) - 2451545.0;

% Mean longitude of the Sun
meanLongitudeSunDegrees = 280.4606184 + ...
	((36000.77005361 / 36525) * fractionalDay); %(degrees)

% Mean anomaly of the Sun
meanAnomalySunDegrees = 357.5277233 + ...
	((35999.05034 / 36525) * fractionalDay); %(degrees)
meanAnomalySunRadians = meanAnomalySunDegrees * pi / 180;

% Ecliptic longitude of the Sun
eclipticLongitudeSunDegrees = meanLongitudeSunDegrees + ...
	(1.914666471 * sin(meanAnomalySunRadians)) + ...
	(0.918994643 * sin(2 * meanAnomalySunRadians)); %(degrees)
eclipticLongitudeSunRadians = eclipticLongitudeSunDegrees * pi / 180;

% Obliquity of the ecliptic plane formula mostly derived from:
% https://en.wikipedia.org/wiki/Ecliptic#Obliquity_of_the_ecliptic
epsilonDegrees = ... % Formula deals with time denominated in centuries
	23.43929 - ((46.8093 / 3600) * fractionalDay / 36525);  %(degrees)
epsilonRadians = epsilonDegrees * pi / 180;

% **** Calculations for unit vectors direction to the Sun are in ECI ****
% Now calculate the Earth to Sun unit vector(s) USING RADIANS, NOT DEGREES
unitEarthSunVectorTransposed = [cos(eclipticLongitudeSunRadians).';
    (cos(epsilonRadians) .* sin(eclipticLongitudeSunRadians)).';
    (sin(epsilonRadians) .* sin(eclipticLongitudeSunRadians)).'];
unitEarthSunVector = unitEarthSunVectorTransposed.';

% Scale up from unit vector(s); distances vary--Earth's orbit is elliptical
distEarthToSunInAstronomicalUnits = ...
	1.000140612 - ...
	(0.016708617 * cos(meanAnomalySunRadians)) - ...
	(0.000139589 * cos(2 * meanAnomalySunRadians)); % in Astronomical Units
distEarthToSunInMeters = ... % (new international definition for an A. U.)
	149597870700 * distEarthToSunInAstronomicalUnits;

% Multiply unit vector(s) by distance to Sun for scaling to full magnitude
vector = ... % Use meters for generic code compatibility
	distEarthToSunInMeters .* unitEarthSunVector; 

end % End of function
