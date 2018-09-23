#include "kalman_filter.h"

using Eigen::MatrixXd;
using Eigen::VectorXd;

// Please note that the Eigen library does not initialize 
// VectorXd or MatrixXd objects with zeros upon creation.

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
  x_ = F_ * x_;
  P_ = F_ * P_ * F_.transpose() + Q_;
}

void KalmanFilter::Update(const VectorXd &z) {
  MatrixXd Ht = H_.transpose();
  MatrixXd I = MatrixXd::Identity(x_.size(), x_.size());

  VectorXd z_pred = H_ * x_;
  VectorXd y = z - z_pred;
  MatrixXd S = H_ * P_ * Ht + R_;
  MatrixXd K = P_ * Ht * S.inverse();

  x_ = x_ + (K * y);
  P_ = (I - K * H_) * P_;
}

void KalmanFilter::UpdateEKF(const VectorXd &z) {
  float px = x_(0);
  float py = x_(1);
  float vx = x_(2);
  float vy = x_(3);

  float rho = sqrt(pow(px, 2) + pow(py, 2));
  float phi = atan2(py, px);
  float rho_dot = (px * vx + py * vy) / rho;

  // bounds and division by zero error checking
  if (fabs(rho) < 0.00001) {
      px += 0.001;
      py += 0.001;
      rho = sqrt(pow(px, 2) + pow(py, 2));
  }
  while (phi > 2 * M_PI) {
      phi -= 2 * M_PI;
  }
  while (phi < -2 * M_PI) {
      phi += 2 * M_PI;
  }

  VectorXd z_pred = VectorXd(3, 1);
  z_pred << rho,
      phi,
      rho_dot;

  MatrixXd Ht = H_.transpose();
  MatrixXd I = MatrixXd::Identity(x_.size(), x_.size());

  VectorXd y = z - z_pred;
  MatrixXd S = H_ * P_ * Ht + R_;
  MatrixXd K = P_ * Ht * S.inverse();

  x_ = x_ + (K * y);
  P_ = (I - K * H_) * P_;
}
