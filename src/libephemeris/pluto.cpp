/*
  This Pluto ephemeris is based on 
  Chapront J., 1995, A&AS 109, 181  (1995A&AS..109..181C)

  The code is translated from the FORTRAN code at
  http://adc.gsfc.nasa.gov/adc-cgi/cat.pl?/catalogs/6/6088/
 */

#include <cstdio>
#include <cmath>
using namespace std;

void
pluto(const double JD, double &Px, double &Py, double &Pz, 
      double &Vx, double &Vy, double &Vz)
{
    const double x = 2*(JD-2341972.5)/146120 - 1;
    const double Fx = x*73060;

    // number of frequencies per coordinate
    static int nf[] = { 82, 19, 5 };

    // Secular Series
    static double ax[] = { 98083308510., -1465718392., 
			   11528487809., 55397965917. };
    static double ay[] = { 101846243715.,      57789., 
			   -5487929294.,  8520205290. };
    static double az[] = { 2183700004.,   433209785.,
			   -4911803413., -14029741184. };

    // Frequencies
    static double fq[] = 
	{
	    0.0000645003954767,0.0001083248054773,0.0001302772403167,
	    0.0001647868659960,0.0001935009111902,0.0002223740247147,
	    0.0003032575201026,0.0003259246239385,0.0003564763034914,
	    0.0004265811293132,0.0004503959517513,0.0004638675148284,
	    0.0005009272733421,0.0005163593863414,0.0005578826828210,
	    0.0005882795362847,0.0006450023602974,0.0007097635821639,
	    0.0007630643253588,0.0007740033551209,0.0008385031396726,
	    0.0008950591609720,0.0009545118163938,0.0010255417569600,
	    0.0010826728325744,0.0011680358909203,0.0012405125052369,
	    0.0012931805883876,0.0013460706181008,0.0014190059530383,
	    0.0014394705053002,0.0014502634075377,0.0014992014575181,
	    0.0015434430430867,0.0016000710611098,0.0016562809940875,
	    0.0017275924266291,0.0017454042542465,0.0018215079641428,
	    0.0018694826929211,0.0019274630193251,0.0020276790928706,
	    0.0021822818660433,0.0022885289854970,0.0023167646379420,
	    0.0023445464874575,0.0024069306189938,0.0024473146628449,
	    0.0024778027974419,0.0025244208011161,0.0025682157855485,
	    0.0026028617439482,0.0026544444009919,0.0026987455959123,
	    0.0027308225916697,0.0027735113723168,0.0028728385464030,
	    0.0029001725379479,0.0029379670182566,0.0029750359447782,
	    0.0031326820696785,0.0031822107498712,0.0031931048857857,
	    0.0032268922327691,0.0034657232066225,0.0037838581645670,
	    0.0038055149432355,0.0038631344783149,0.0039129259467328,
	    0.0040311445462510,0.0040607542008930,0.0041490103414206,
	    0.0043500678052272,0.0046321937641054,0.0058000376725240,
	    0.0091460971544658,0.0091560629947357,0.0172021239411871,
	    0.0182919855069063,0.0279624510118796,0.0344040996177640,
	    0.0714245719830324,0.0001083248054773,0.0001647868659960,
	    0.0003032575201026,0.0003259246239385,0.0003564763034914,
	    0.0005009272733421,0.0005882795362847,0.0007097635821639,
	    0.0007630643253588,0.0008950591609720,0.0011680358909203,
	    0.0014502634075377,0.0017454042542465,0.0029001725379479,
	    0.0031822107498712,0.0040607542008930,0.0043500678052272,
	    0.0091460971544658,0.0279624510118796,0.0001083248054773,
	    0.0003032575201026,0.0011680358909203,0.0043500678052272,
	    0.0279624510118796
	};

    //  Coefficients X (cosine)
    static double cx[] =
	{
	    -16338582222., -5995086437.,  23663880362., 10304632056.,
	    -3996936944., -4136465568.,   1188702881.,  -621434363.,
	    566898160.,   -75880391.,    576146406.,  -659684298.,
	    451962774.,  -153724334.,   -603163280.,   364764379.,
	    193062130.,   161493959.,   1167349082., -1417467887.,
	    15325240.,    -3624391.,      -587306.,      132022.,
	    -106501.,      228373.,       -95106.,       56299.,
            -48339.,      803937.,     -6172744.,   -18962749.,
            133022.,      -25964.,         7111.,       -4998.,
	    32034.,      -29666.,        -1983.,         114.,
	    191.,       -1063.,          419.,         346.,
	    5059.,         -81.,         1408.,        2964.,
	    -5364.,        1509.,        -4924.,        2954.,
	    2034.,       -5199.,          604.,       -1247.,
	    4576.,     -350741.,        -4023.,        1147.,
	    -38.,         -99.,       -11686.,        1129.,
	    582.,         -83.,          -97.,         431.,
	    -134.,        -323.,         -292.,         195.,
	    39068.,         523.,        -1747.,        3135.,
	    -619.,      -12095.,            6.,       18476.,
	    -130.,        -438., 102345278799., -9329130892.,
	    1484339404.,   472660593.,   -581239444.,  1016663241.,
	    -1054199614.,    99039105.,    -52190030.,    -3394173.,
            -16529.,     3102430.,         2286.,      -10955.,
	    -5293.,        -654.,          124.,         -85.,
	    29.,   418209651.,  -1191875710.,     -823081.,
	    -558.,       -1091.
	};

    // Coefficients X (sine)
    static double sx[] =
	{
	    -308294137468.,-68820910480.,  28346466257., -1755658975.,
	    7818660837., -1098895702.,  -1192462299.,  -772129982.,
	    1061702581.,  -639572722.,   1128327488.,  -423570428.,
	    -175317704.,   251601606.,   -869448807.,   551228298.,
	    87807522.,   -11540541.,   -103236703.,    92638954.,
	    -3624991.,     1004975.,       304396.,      -56532.,
	    55554.,     -799096.,        56947.,      -48016.,
	    50599.,     -680660.,      5858452.,    38125648.,
	    -109460.,       18684.,        -5269.,        2771.,
	    -6814.,       47130.,         1192.,       -1387.,
	    379.,        -612.,          -52.,         813.,
	    -4354.,       -2275.,          685.,       -1352.,
	    4681.,       -1908.,        -6530.,        8667.,
	    1675.,         874.,          898.,         965.,
	    -7124.,    -1145389.,         2931.,        -618.,
	    -34.,       -6562.,         8038.,        -697.,
	    -8.,          12.,         -267.,        -131.,
	    304.,        -756.,         -103.,        -250.,
	    19816.,        -596.,          576.,        4122.,
	    65.,      -27900.,          217.,        -137.,
	    -269.,         531., -24338350765., 11210995713.,
	    2793567155.,  -776019789.,   1528323591.,  -249354416.,
	    1127608109.,  -667692329.,  -1570766679.,    -9724425.,
	    26552.,     3332520.,       -27607.,      -11696.,
	    -7297.,        -104.,         -184.,        -455.,
	    -16., 39813894679.,   3633087275.,      522728.,
	    -320.,       -1401.
	};

    // Coefficients Y (cosine)
    static double cy[] = 
	{
	    299584895562., 75951634908., -36135662843., 18125610071.,
	    -20398008415.,  6125780503.,   -162559485.,  4352425804.,
	    -3819676998.,  1168107376.,  -5041323701.,  4093828501.,
	    -1727274544.,   134214260.,   5033950069., -3071449401.,
	    -1190419055.,  -775881742.,  -5524713888.,  6803228005.,
	    -65675611.,    15155413.,      2009509.,     -389682.,
            275571.,      474366.,       132163.,      -81550.,
	    69996.,     -706470.,      4777898.,   -44002785.,
            -58735.,        7624.,        -1922.,        -729.,
	    -1733.,      -35642.,         -586.,        -258.,
	    -368.,        1286.,         -136.,         883.,
	    2673.,         331.,           50.,         178.,
	    2901.,        -654.,        -8972.,        3034.,
	    1113.,         570.,          -72.,        1950.,
	    8550.,     1047593.,        -2348.,         313.,
	    432.,        6765.,        -8240.,         335.,
	    140.,        -833.,          252.,        -210.,
	    366.,        -920.,         1215.,        -217.,
            -17780.,         581.,         -560.,       -4131.,
	    390.,       25613.,         -206.,        1850.,
	    171.,        -471.,  26437625772.,-12674907683.,
	    -1067899665.,    -2082744.,    -43195632.,   211912497.,
	    -108307161.,   -63033809.,   -203850703.,    -1672332.,
	    7136.,      803655.,       -10985.,        9126.,
	    3317.,        -151.,          160.,         138.,
	    -27.,-36463065062.,  -5816560445.,     1576292.,
	    -21.,        -295.
	};

    // Coefficients Y (sine)
    static double sy[] =
	{
      -53545027809., -8838029861.,  23553788174., 13775798112.,
       -6068121593., -2853107588.,    750355551.,   -82067770.,
         230091832.,  -259838942.,    197944074.,    27141006.,
        -105334544.,    95175918.,   -139461973.,    80593104.,
          -5126842.,   -21953793.,   -163767784.,   192436228.,
          -2479113.,      561687.,       121909.,      -30275.,
             16333.,       68105.,        24081.,      -11228.,
               667.,      -73047.,      1007089.,   -22814549.,
               434.,        1013.,          710.,        1100.,
             -4598.,        1990.,          564.,         828.,
             -1119.,       -1249.,         -597.,         227.,
              5467.,         801.,        -2029.,       -1892.,
              4713.,        -459.,         1757.,       -9303.,
             -2357.,        7679.,        -2953.,         629.,
              5011.,     -333905.,        -2388.,         415.,
               139.,       -5726.,        -4583.,         310.,
               681.,        -107.,          301.,        -525.,
               198.,        -379.,         -230.,         -64.,
             36069.,         459.,        -1596.,        2509.,
              -146.,      -11081.,            4.,       15764.,
              -147.,        -362., 117449924600., -7691661502.,
       -4771148239.,  3733883366.,  -7081845126.,  3502526523.,
       -8115570206.,  3607883959.,   7690328772.,    37384011.,
           -164319.,    -2859257.,         1593.,      -11997.,
             -6476.,        1419.,           34.,         232.,
                32.,  2752753498.,   -672124207.,      154239.,
              -400.,         372.	    
	};

    // Coefficients Z (cosine)
    static double cz[] = 
	{
	    98425296138., 25475793908., -18424386574.,  2645968636.,
	    -5282207967.,  3278235471.,   -425422632.,  1526641086.,
	    -1323182752.,   235873266.,  -1617466723.,  1557465867.,
	    -848586296.,   218182986.,   1636044515., -1001334243.,
	    -455739370.,  -348173978.,  -2511254281.,  3062521470.,
	    -32079379.,     7597939.,      1138566.,     -238849.,
            192377.,       83169.,       148694.,      -92489.,
	    87116.,    -1281070.,      9950106.,   -25105642.,
	    -171749.,       31035.,        -8648.,        5360.,
            -30345.,       11482.,         1322.,        -467.,
	    96.,         894.,         -381.,        -583.,
	    2525.,        -569.,          226.,       -2039.,
	    3728.,       -1540.,           42.,       -3144.,
	    658.,         220.,         1848.,         678.,
	    -7289.,      463291.,         3945.,       -1141.,
	    -26.,      -10607.,        11458.,       -1005.,
	    120.,        -301.,          135.,        -186.,
	    118.,          30.,          197.,        -182.,
	    -8585.,         240.,         -226.,       -2049.,
	    283.,       11109.,         -100.,        -842.,
	    71.,        -181., -22591501373., -1138977908.,
	    -782718600.,  -141483824.,    159033355.,  -246222739.,
	    287284767.,   -48002332.,    -41114335.,      578004.,
	    -8420.,     -766779.,          957.,        5780.,
	    4141.,         417.,           -8.,          65.,
	    -22.,-11656050047.,  -1186276469.,     1388681.,
	    201.,         561.
	};

    // Coefficients Z (sine)
    static double sz[] = 
	{
	    76159403805., 17987340882.,  -1193982379.,  4828308190.,
	    -4248985438.,  -559147671.,    593594960.,   208799497.,
	    -249913200.,   115051024.,   -282588988.,   135883560.,
	    23091693.,   -49187976.,    223956575.,  -137344299.,
	    -28188872.,    -2636274.,    -14202661.,    25488216.,
            419837.,     -150966.,       -64906.,        3719.,
	    -2226.,       86321.,       -15970.,       16609.,
            -15782.,      200300.,     -1500491.,    -9161491.,
	    37481.,       -4616.,          224.,       -1027.,
	    5220.,       -6976.,         -267.,         556.,
	    -23.,        -711.,         -122.,         -97.,
	    2440.,         786.,         -806.,        -167.,
	    -156.,         572.,         2532.,       -4582.,
	    -1178.,         875.,         -558.,         781.,
	    3230.,     -116132.,        -1440.,         438.,
	    176.,        1072.,        -5850.,         418.,
	    267.,          60.,          134.,         -85.,
	    -59.,         112.,         -168.,         -89.,
	    14986.,         190.,         -685.,        1018.,
	    -48.,       -4807.,            0.,        7066.,
	    -54.,        -229.,  44126663549., -5626220823.,
	    -2536450838.,  1536292657.,  -2916144530.,   949074586.,
	    -2842935040.,  1500396857.,   3415136438.,    19702076.,
            -46995.,    -5801645.,        33470.,       17674.,
	    7355.,         199.,           11.,         205.,
	    33.,-11127973411.,  -1310869292.,     -164753.,
	    -107.,         284.
	};	    

    // Compute the positions (secular terms)
    Px = 0;
    Py = 0;
    Pz = 0;
    double wx = 1;
    for (int i = 0; i < 4; i++)
    {
	Px += ax[i] * wx;
	Py += ay[i] * wx;
	Pz += az[i] * wx;
	wx *= x;
    }

    // Compute the positions (periodic and Poisson terms)
    int imax = 0;
    wx = 1;
    double w[3];
    for (int m = 0; m < 3; m++)
    {
	for (int iv = 0; iv < 3; iv++) w[iv] = 0;
	int imin = imax;
	imax += nf[m];
	for (int i = imin; i < imax; i++)
	{
	    double f = fq[i] * Fx;
	    double cf = cos(f);
	    double sf = sin(f);
	    w[0] += cx[i] * cf + sx[i] * sf;
	    w[1] += cy[i] * cf + sy[i] * sf;
	    w[2] += cz[i] * cf + sz[i] * sf;
	}
	Px += w[0] * wx;
	Py += w[1] * wx;
	Pz += w[2] * wx;
	wx *= x;
    }

    Px /= 1e10;
    Py /= 1e10;
    Pz /= 1e10;

    // Compute the velocities (secular terms)
    double wt = 1./73060.;
    wx = 1;
    for (int i = 1; i < 4; i++)
    {
	Vx += i * ax[i] * wx;
	Vy += i * ay[i] * wx;
	Vz += i * az[i] * wx;
	wx *= x;
    }
    Vx *= wt;
    Vy *= wt;
    Vz *= wt;

    // Compute the velocities (periodic and Poisson terms)
    imax = 0;
    wx = 1;
    double wy;
    for (int m = 0; m < 3; m++)
    {
	int imin = imax;
	imax += nf[m];
	for (int i = imin; i < imax; i++)
	{
	    double fw = fq[i];
	    double f = fw * Fx;
	    double cf = cos(f);
	    double sf = sin(f);
	    Vx += fw * (sx[i] * cf - cx[i] * sf) * wx;
	    Vy += fw * (sy[i] * cf - cy[i] * sf) * wx;
	    Vz += fw * (sz[i] * cf - cz[i] * sf) * wx;
	    if (m > 0)
	    {
		Vx += m * wt * (cx[i] * cf + sx[i] * sf) * wy;
		Vy += m * wt * (cy[i] * cf + sy[i] * sf) * wy;
		Vz += m * wt * (cz[i] * cf + sz[i] * sf) * wy;
	    }
	}
	wy = wx;
	wx *= x;
    }

    Vx /= 1e10;
    Vy /= 1e10;
    Vz /= 1e10;
}

#if 0
/*
  Test values:

  Date:  Jan  01   1700    H : 00h 00m 00s (TDB)    JD:   2341972.5000000
  X :-25.48366603086599    Y : 22.25190224179014    Z : 14.61666566142614   au
  X': -0.00140296544832    Y': -0.00253543942176    Z': -0.00036577359317   au/d
  
  Date:  Jan  02   1800    H : 06h 00m 00s (TDB)    JD:   2378497.7500000
  X : 36.33316699469712    Y :-11.84871881208418    Z :-14.64079073464049   au
  X':  0.00151098228705    Y':  0.00214812030172    Z':  0.00021249511616   au/d
  
  Date:  Jan  03   1900    H : 12h 00m 00s (TDB)    JD:   2415023.0000000
  X : 10.29158303131287    Y : 44.52906466047693    Z : 10.79081191605171   au
  X': -0.00216104614307    Y': -0.00004877516272    Z':  0.00063748726618   au/d
  
  Date:  Jan  04   2000    H : 18h 00m 00s (TDB)    JD:   2451548.2500000
  X : -9.86615874601937    Y :-27.98285304568784    Z : -5.75779357947923   au
  X':  0.00302900782509    Y': -0.00112671144850    Z': -0.00126494662037   au/d
  
  Date:  Jan  05   2100    H : 00h 00m 00s (TDB)    JD:   2488073.5000000
  X : 39.67448463874504    Y : 28.47968765660414    Z : -3.06796133066342   au
  X': -0.00097971861494    Y':  0.00171018575529    Z':  0.00082844820875   au/d
*/
int
main(int argc, char **argv)
{
    double Px, Py, Pz;
    double Vx, Vy, Vz;
    pluto(2341972.5, Px, Py, Pz, Vx, Vy, Vz);
    pluto(2378497.75, Px, Py, Pz, Vx, Vy, Vz);
    pluto(2415023., Px, Py, Pz, Vx, Vy, Vz);
    pluto(2451548.25, Px, Py, Pz, Vx, Vy, Vz);
    pluto(2488073.5, Px, Py, Pz, Vx, Vy, Vz);

    printf("JD %20.8f:\n%20.16f%20.16f%20.16f\n%20.16f%20.16f%20.16f\n",
	   JD, Px, Py, Pz, Vx, Vy, Vz);
}
#endif
