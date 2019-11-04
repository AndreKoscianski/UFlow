
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <chrono>
#include <time.h>

struct SReport {

   int
       resolutionX
      ,resolutionY
      ,enlR_n_loop
      ,enlR_pix_before
      ,enlR_pix_after
      ,truePositive
      ,trueNegative
      ,falsePositive
      ,falseNegative
      ,Nurban_img1
      ,Nurban_img2
      ,Ncluster1
      ,Ncluster2
      ,minClusterSize
      ,sim_pixels
      ,spray_n_clusters
      ,expected_im3_size
      ,forecast_organic_pixels
      ,forecast_total_pixels
      ,forecast_loops
      ,sim_loops
      ;

   double
       metricLeeSallee
      ,metricMatthews
      ,deltaTime
      ,forecastTime
      ,minK
      ,maxK
      ,KMapNoise
      ;

   std::string ClusterList1, ClusterList2;

   std::string TStartExecution, TStartCalibration, TStartForecast, TEndExecution;

   std::chrono::duration<double> TCalibration, TForecast, TTotal;

} GReport;


//-----------------------------------------------------------
// From "Rashad", Dhaka, Bangladesh
const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);

    strftime (buf, sizeof(buf), "%d/%b/%Y (%a) %Hh%M", &tstruct);

    return buf;
}


//-----------------------------------------------------------
void GenerateReport (const char *path) {

  std::ofstream arq;

  arq.open (path);

  if (arq.fail()) return;

  arq << "\n   UFlow results\n"
           "===================\n\n"
      ;

  arq << "Start Execution   " << GReport.TStartExecution   << "\n";
//  arq << "Start Calibration " << GReport.TStartCalibration << " (next uflow version, print delta time)\n";
//  arq << "Start Forecast    " << GReport.TStartForecast    << "\n";
//  arq << "End Execution     " << GReport.TEndExecution     << "\n\n";

  arq << "Calibration time = " << GReport.TCalibration.count() << " seconds\n";
  arq << "Forecast    time = " << GReport.TForecast   .count() << " seconds\n";
  arq << "Total Exec  time = " << GReport.TTotal      .count() << " seconds\n\n";



  arq << "---------------------------------------\n"
         "     Inputs and  Parameters\n"
         "---------------------------------------\n\n";

  arq << "Input Files----------------\n"
      << " Resolution ("
      << GReport.resolutionX << " x "
      << GReport.resolutionY << ")\n"
      << " image1    = " <<    config_gets("image1")    << "\n"
      << " image2    = " <<    config_gets("image2")    << "\n"
      << " roads     = " <<    config_gets("roads")     << "\n"
      << " exclusion = " <<    config_gets("exclusion") << "\n\n";

  arq << "Time between images 1 and 2 = " << GReport.deltaTime << " years\n\n";

  arq << "Pi Map: enlarge Roads+Image2 by " <<  (int)config_getd ("HeatCyclesPiMap") << " cycles \n\n";

  arq << "KMap noise = " <<  config_getd ("KMapNoise") << "\n\n";

  arq << "Max heat equation cycles = " <<  (int) config_getd ("MaxHeatCycles") << "\n\n";

  arq << "Max calib loops = " <<  (int) config_getd ("MaxCalibrationLoops") << "\n\n";

  arq << "gamma (controls kappa \'insulation\') = " << config_getd ("gamma") << "\n\n";

  arq << "Urban pixels,\n"
      << " image1 = " << GReport.Nurban_img1 << "\n"
      << " image2 = " << GReport.Nurban_img2
      << " (+ "
      << std::setprecision (3)
      << 100.0 * (GReport.Nurban_img2 - GReport.Nurban_img1) / GReport.Nurban_img1
      << " %)\n\n";

  arq << "Number of clusters,\n"
      << " image1 = " << GReport.Ncluster1 << "\n"
      << " image2 = " << GReport.Ncluster2 << "\n\n";

  arq << "List of clusters, first image:\n" 
      << GReport.ClusterList1
      << "\n\n";

  arq << "List of clusters, second image:\n" 
      << GReport.ClusterList2
      << "\n\n";

  arq << "Avg. cluster size = " << GReport.minClusterSize << " (smaller ones)\n\n";



  arq << "---------------------------------------\n"
         "     Calculations \n"
         "---------------------------------------\n\n";

   arq << "Output Files----------------\n"
       << " simulation = " <<    config_gets("simulationImg") << "\n"
       << " sim error  = " <<    config_gets("simulationErr") << "\n"
       << " forecast   = " <<    config_gets("forecastImg")   << "\n"
       << " final KMap = " <<    config_gets("SaveKMap")      << "\n\n";

/*
  arq << "Map for spontaneous (random) grow.\n "
      <<  GReport.enlR_n_loop     << " loops to enlarge roads,\n "
      <<  GReport.enlR_pix_before << " original pixels,\n "
      <<  GReport.enlR_pix_after  << " pixels after expansion.\n\n";
*/
  arq << "K Map, extreme values.\n"
      << "  min  = " << GReport.minK  << "\n"
      << "  max  = " << GReport.maxK  << "\n\n";

  arq << "Simulation: final metrics.\n"
      << "  Urban pixels = " << GReport.sim_pixels
      << " (+ "
      << std::setprecision (3)
      << 100.0 * (GReport.sim_pixels - GReport.Nurban_img1) / GReport.Nurban_img1
      << " % over 1st image)\n"
      << "  Heat cycles  = " << GReport.sim_loops       << "\n"
      << "  Lee Sallee   = " << GReport.metricLeeSallee << "\n"
      << "  Matthews     = " << GReport.metricMatthews  << "\n";

  arq << "    True  Positive / Negative = "
      << GReport.truePositive << " / " << GReport.trueNegative << "\n"
      << "    False Positive / Negative = "
      << GReport.falsePositive << " / " << GReport.falseNegative << "\n\n";

  arq << "---------------------------------\n"
      << "       Forecast\n"
         "---------------------------------\n\n";

  arq << "Time elapsed (from second image) = " << GReport.forecastTime << " years\n";

  arq << "Heat cycles         = " << GReport.forecast_loops << " \n";

  arq << "Lin. regression     = " << GReport.expected_im3_size << " (expected new city size)\n";

  arq << "Area new cluster    = " << GReport.minClusterSize << "\n";

  arq << "Number new clusters = " << GReport.spray_n_clusters << "\n";

  int nauc = GReport.minClusterSize * GReport.spray_n_clusters;

  arq << "Expansion (new pixels):\n";

  arq << " clusters           = " << nauc << " ("
      << (100.0 * nauc) / GReport.forecast_total_pixels << "% of total)\n";
  
  arq << " city sprawl        = " <<  GReport.forecast_organic_pixels << " \n";

  arq << "Forecast city size  = " <<  GReport.forecast_total_pixels 
      << " (+ "
      << std::setprecision (3)
      << 100.0 * (GReport.forecast_total_pixels - GReport.Nurban_img2) / GReport.Nurban_img2
      << " % over 2nd image)\n\n";


  arq.close();


}
