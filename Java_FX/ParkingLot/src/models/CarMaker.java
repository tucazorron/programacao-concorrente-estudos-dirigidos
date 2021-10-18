package models;

import controllers.MainController;

import java.util.Random;
import java.util.concurrent.Semaphore;

public class CarMaker implements Runnable{

    private Semaphore mutex;
    private Semaphore parkingPen;
    private Semaphore access;

    private MainController controller;

    private boolean[] parkingLots;
    private int carQuantity;
    private Random random;

    public CarMaker(Semaphore mutex, Semaphore parkingPen, Semaphore access, boolean[] parkingLots, int carQuantity, MainController controller){
        this.mutex = mutex;
        this.parkingPen = parkingPen;
        this.parkingLots = parkingLots;
        this.carQuantity = carQuantity;
        this.access = access;
        this.controller = controller;
        random = new Random(System.currentTimeMillis());
    }

    @Override
    public void run() {
        //Creating the cars
        Car car;
        for(int i=0;i<carQuantity;i++){
            car = new Car(mutex,parkingPen,access,parkingLots);
            car.addObserver(controller);
            new Thread(car,"CAR" + (i+1)).start();
            try {
                Thread.sleep(random.nextInt(401) + 100);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
