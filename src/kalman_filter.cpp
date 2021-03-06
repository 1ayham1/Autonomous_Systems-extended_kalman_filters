#include "kalman_filter.h"

using Eigen::MatrixXd;
using Eigen::VectorXd;

KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in, MatrixXd &F_in,
                        MatrixXd &H_in, MatrixXd &R_in, MatrixXd &Q_in) {
  x_ = x_in;
  P_ = P_in;
  F_ = F_in;
  H_ = H_in;
  R_ = R_in;
  Q_ = Q_in;
}

void KalmanFilter::Predict() {
  /**
    * predict the state
  */

  x_ = F_ * x_;
	MatrixXd Ft = F_.transpose();
	P_ = F_ * P_ * Ft + Q_;

}

void KalmanFilter::Update(const VectorXd &z) {
  /**
    * update the state by using Kalman Filter equations
  */

  VectorXd z_pred = H_ * x_;
	VectorXd y = z - z_pred;
	MatrixXd Ht = H_.transpose();
	MatrixXd S = H_ * P_ * Ht + R_;
	MatrixXd Si = S.inverse();
	MatrixXd PHt = P_ * Ht;
	MatrixXd K = PHt * Si;

	//new estimate
	x_ = x_ + (K * y);
	long x_size = x_.size();
	MatrixXd I = MatrixXd::Identity(x_size, x_size);
	P_ = (I - K * H_) * P_;


}

void KalmanFilter::UpdateEKF(const VectorXd &z) {
  /**
    * update the state by using Extended Kalman Filter equations
  */

  float px = x_[0] , py = x_[1], vx = x_[2], vy = x_[3];

  //convert to polar and repeat the above steps
  float rho = sqrt(px*px + py*py);
  float phi = atan2(py, px); // for stability: normalization hint from the forum.
  float rho_dot;

  if(rho < 0.0000001)
    rho_dot = 0;
  else
    rho_dot = (px*vx + py*vy)/rho;


  VectorXd hx_prime = VectorXd(3);
  hx_prime << rho, phi, rho_dot;

  //-------------------------------------

	VectorXd y = z - hx_prime;
  //suggested normalization and nice discussion
  //https://discussions.udacity.com/t/working-model-with-simulator-but-values-a-pretty-high/321176
  //https://discussions.udacity.com/t/already-used-atan2-to-calculate-phi-in-hx-do-i-still-need-to-normalize-the-phi-in-y/242332/2

  y(1)= atan2(sin(y(1)),cos(y(1)));

	MatrixXd Ht = H_.transpose();
	MatrixXd S = H_ * P_ * Ht + R_;
	MatrixXd Si = S.inverse();
	MatrixXd PHt = P_ * Ht;
	MatrixXd K = PHt * Si;

	//new estimate
	x_ = x_ + (K * y);
	long x_size = x_.size();
	MatrixXd I = MatrixXd::Identity(x_size, x_size);
	P_ = (I - K * H_) * P_;

}
