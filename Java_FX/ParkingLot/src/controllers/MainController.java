package controllers;

import javafx.animation.RotateTransition;
import javafx.animation.SequentialTransition;
import javafx.animation.TranslateTransition;
import javafx.application.Platform;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.*;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.StackPane;
import javafx.scene.paint.Color;
import javafx.scene.paint.Paint;
import javafx.scene.shape.Circle;
import javafx.scene.shape.Line;
import javafx.scene.shape.Rectangle;
import javafx.scene.text.Font;
import javafx.scene.text.Text;
import javafx.stage.StageStyle;
import javafx.util.Duration;
import models.CarMaker;
import models.ParkingPen;

import java.net.URL;
import java.util.*;
import java.util.concurrent.Semaphore;

public class MainController implements Initializable, Observer {

    @FXML
    private AnchorPane anchorPane;
    @FXML
    private Circle spawnVehicle;
    @FXML
    private Circle beforeEntrance;
    @FXML
    private Circle afterEntrance;
    @FXML
    private Circle parking1;
    @FXML
    private Button startButton;
    @FXML
    private Button skipButton;
    @FXML
    private Button helpButton;
    @FXML
    private ComboBox<String> comboCars;
    @FXML
    private Label carsInsideLabel;

    private Thread carCreator;
    private SequentialTransition finalAnimation;

    private ArrayList<Object> objectsInAnchor;
    private HashMap<String,StackPane> carMap;

    private int carQuantity = 100;
    private long parkingLots = Math.round(carQuantity*.2);

    private String finalMessage;

    private int counter = 0;

    private double animationLength = 0.1;


    @Override
    public void initialize(URL url, ResourceBundle resourceBundle) {
        objectsInAnchor = new ArrayList<>();
        carMap = new HashMap<>();
        finalAnimation = new SequentialTransition();
        finalMessage = "";
        skipButton.setDisable(true);
        carsInsideLabel.setText("Cars inside: " + ParkingPen.carsInside);


        comboCars.getItems().add("" + 5);
        for(int i=1;i<=10;i++){
            comboCars.getItems().add("" + (10 * i));
        }

        comboCars.valueProperty().addListener(new ChangeListener<String>() {
            @Override
            public void changed(ObservableValue<? extends String> observableValue, String oldValue, String newValue) {
                if( !(newValue.isEmpty() || newValue.isBlank()) ){
                    carQuantity =  Integer.parseInt(newValue);
                    parkingLots = Math.round(carQuantity*.2);
                    anchorPane.getChildren().removeAll(objectsInAnchor);
                    objectsInAnchor.clear();
                    createParkingLots();
                }
            }
        });
        comboCars.valueProperty().setValue("100");

    }


    @Override
    public void update(Observable o, Object arg) {
        Platform.runLater(() -> {
            String[] result = ((String)arg).split(" ");
                switch (result[0]){
                    case "ENTER":
                        entranceAnimation(result[1],Integer.parseInt(result[2]),ParkingPen.carsInside);
                        finalMessage += result[1] + " ENTRÓ AL ESTACIONAMIENTO\n";
                    break;

                    case "EXIT":
                        exitAnimation(result[1],Integer.parseInt(result[2]),ParkingPen.carsInside);
                        counter++;
                        double totalTime = Double.parseDouble(result[3])/1000;
                        int parkingLot = Integer.parseInt(result[2]) + 1;
                        finalMessage += result[1] + " SALIÓ DEL ESTACIONAMIENTO Y ESTUVO ESTACIONADO POR: " + totalTime + " SEGUNDOS, EN EL LUGAR: " + parkingLot + "\n";
                    break;
                }
            if(counter == carQuantity){
                playAnimation();
            }
        });
    }

    @FXML
    public void startSimulation(ActionEvent event){
        finalMessage = "";
        //carMap = new HashMap<>();
        //finalAnimation = new SequentialTransition();
        startButton.setDisable(true);
        comboCars.setDisable(true);
        String title= "INFORMACION";
        String message = "La simulación es para representar el orden de entrada y salida de cada vehículo al estacionamiento.\n" +
                "No representa el tiempo real de espera, toda la ejecución se mostrará al final en una alerta.\n" +
                "ATENCIÓN: Sólo podrá hacer skip de la animación en cuanto el primer auto comience a ingresar en el estacionamiento.";
        showFirstAlert(title,message, Alert.AlertType.INFORMATION);

        int parkings = Integer.parseInt("" + parkingLots);
        Semaphore mutex = new Semaphore(1);
        Semaphore parkingPen = new Semaphore(parkings);
        Semaphore access = new Semaphore(0);
        boolean[] parkingLots = new boolean[parkings];
        for(int i=0;i<parkingLots.length;i++){
            parkingLots[i] = false;
        }
        CarMaker carMaker = new CarMaker(mutex,parkingPen,access,parkingLots,carQuantity,this);
        carCreator = new Thread(carMaker);
        carCreator.start();

    }

    public void createParkingLots(){
        int layoutY = 0;
        for(int i=0;i<parkingLots+1;i++){
            Line parkingLine;
            parkingLine = new Line();
            parkingLine.setStartX(-100);
            parkingLine.setEndX(100);
            parkingLine.setLayoutX(672);
            parkingLine.setLayoutY(layoutY);
            parkingLine.setStrokeWidth(5);
            parkingLine.setFill(Color.YELLOW);
            parkingLine.setStroke(Color.YELLOW);
            layoutY += 50;
            anchorPane.getChildren().add(parkingLine);
            objectsInAnchor.add(parkingLine);
        }

        layoutY = 15;

        for(int i=0;i<parkingLots;i++){
            Label parkingNumber;
            parkingNumber = new Label("" + (i+1));
            parkingNumber.setTextFill(Color.WHITE);
            parkingNumber.setLayoutX(670);
            parkingNumber.setFont(Font.font("Arial Black",15));
            parkingNumber.setLayoutY(layoutY);
            parkingNumber.setRotate(90);
            layoutY += 50;
            anchorPane.getChildren().add(parkingNumber);
            objectsInAnchor.add(parkingNumber);
        }
    }

    public void entranceAnimation(String name, int parkingPosition, int carsInside){
        StackPane car = new StackPane();
        Rectangle carRectangle = new Rectangle();
        carRectangle.setHeight(150);
        carRectangle.setWidth(30);
        car.setLayoutX(spawnVehicle.getLayoutX());
        car.setLayoutY(spawnVehicle.getLayoutY());

        Text text = new Text(name);
        text.setFont(new Font("Arial Black",10));
        text.setFill(Color.BLACK);

        car.getChildren().addAll(carRectangle,text);

        carMap.put(name,car);

        carRectangle.setFill(getRandomColor());
        carRectangle.setStroke(Color.BLACK);
        carRectangle.setStrokeWidth(1);
        anchorPane.getChildren().add(car);

        double newPosition = beforeEntrance.getLayoutY() - spawnVehicle.getLayoutY();

        TranslateTransition spawnTransition = new TranslateTransition();
        spawnTransition.setToY(newPosition);
        spawnTransition.setDuration(Duration.seconds(animationLength));

        newPosition = afterEntrance.getLayoutX() - beforeEntrance.getLayoutX();

        RotateTransition enterRotation = new RotateTransition();
        enterRotation.setByAngle(90);
        enterRotation.setDuration(Duration.seconds(animationLength));


        TranslateTransition enterTransition = new TranslateTransition();
        enterTransition.setToX(newPosition);
        enterTransition.setDuration(Duration.seconds(animationLength));

        RotateTransition insideRotation = new RotateTransition();
        insideRotation.setByAngle(-90);
        insideRotation.setDuration(Duration.seconds(animationLength));

        TranslateTransition firstLotTransition = new TranslateTransition();
        firstLotTransition.setToY(-48 - car.getLayoutY());
        firstLotTransition.setDuration(Duration.seconds(animationLength));

        RotateTransition lotRotation = new RotateTransition();
        lotRotation.setByAngle(90);
        lotRotation.setDuration(Duration.seconds(animationLength));

        newPosition = (parking1.getLayoutX() - car.getLayoutX()) - 90;

        TranslateTransition frontTransition = new TranslateTransition();
        frontTransition.setToX(newPosition);
        frontTransition.setDuration(Duration.seconds(animationLength));
        SequentialTransition sequentialTransition = new SequentialTransition();
        sequentialTransition.setNode(car);
        sequentialTransition.getChildren().addAll(spawnTransition,enterRotation,enterTransition,insideRotation,
                                                  firstLotTransition,lotRotation,frontTransition, chooseParkingLot(car,parkingPosition,carsInside));
        finalAnimation.getChildren().add(sequentialTransition);
    }

    public SequentialTransition chooseParkingLot(StackPane car, int parkingPosition,int carsInside){
        SequentialTransition sequentialTransition = new SequentialTransition();
        sequentialTransition.setNode(car);
        double initialPosition = (-1 * car.getLayoutY()) + 1;
        for(int i=0;i<parkingPosition;i++){
            RotateTransition faceDown = new RotateTransition();
            faceDown.setByAngle(90);
            faceDown.setDuration(Duration.seconds(animationLength));

            TranslateTransition goNext = new TranslateTransition();
            goNext.setToY(initialPosition);
            goNext.setDuration(Duration.seconds(animationLength));

            initialPosition += 50;

            RotateTransition faceLot = new RotateTransition();
            faceLot.setByAngle(-90);
            faceLot.setDuration(Duration.seconds(animationLength));

            sequentialTransition.getChildren().addAll(faceDown,goNext,faceLot);
        }

        TranslateTransition enterLot = new TranslateTransition();
        enterLot.setToX(580);
        enterLot.setDuration(Duration.seconds(animationLength));
        enterLot.setOnFinished(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {
                carsInsideLabel.setText("Cars inside: " + carsInside);
            }
        });

        sequentialTransition.getChildren().add(enterLot);
        return sequentialTransition;
    }

    public Paint getRandomColor() {
        Random random = new Random();
        int r = random.nextInt(255);
        int g = random.nextInt(255);
        int b = random.nextInt(255);
        return Color.rgb(r, g, b);
    }

    public void exitAnimation(String name, int parkingPosition, int carsInside){
        int angle = 0;
        double newPosition = 0;
        StackPane pane = carMap.get(name);
        SequentialTransition sequentialTransition = new SequentialTransition();
        sequentialTransition.setNode(pane);

        TranslateTransition getBack = new TranslateTransition();
        getBack.setToX(392);

        if(parkingPosition < 10){
            angle = 90;
        } else if (angle == 11){
            angle = 0;
        } else {
            angle = -90;
        }

        RotateTransition rotateInPlace = new RotateTransition();
        rotateInPlace.setByAngle(angle);
        rotateInPlace.setDuration(Duration.seconds(animationLength));

        TranslateTransition goToMiddle = new TranslateTransition();
        goToMiddle.setToY((-1 * afterEntrance.getLayoutY()) - 50);
        goToMiddle.setDuration(Duration.seconds(animationLength));

        RotateTransition rotateToExit = new RotateTransition();
        rotateToExit.setByAngle(angle);
        rotateToExit.setDuration(Duration.seconds(animationLength));

        TranslateTransition getOut = new TranslateTransition();
        getOut.setToX(-1000);
        getOut.setDuration(Duration.seconds(animationLength));
        getOut.setOnFinished(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {
                Platform.runLater(()-> {
                    carsInsideLabel.setText("Cars inside: " + carsInside);
                });
            }
        });

        sequentialTransition.getChildren().addAll(getBack,rotateInPlace,goToMiddle,rotateToExit,getOut);
        finalAnimation.getChildren().add(sequentialTransition);
    }

    public void playAnimation(){
        finalAnimation.setOnFinished(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {
                Platform.runLater(() -> {
                    showAlert("FINISHED",finalMessage, Alert.AlertType.CONFIRMATION);
                    reset();
                });
            }
        });
        skipButton.setDisable(false);
        finalAnimation.play();
    }

    @FXML
    public void skipAnimation(ActionEvent event){
        finalAnimation.stop();
        showAlert("FINISHED",finalMessage, Alert.AlertType.CONFIRMATION);
        reset();
    }

    private void reset(){
        counter = 0;
        ParkingPen.carsInside = 0;
        ParkingPen.areEntering = false;
        ParkingPen.areLeaving = false;
        Iterator<String> iterator = carMap.keySet().iterator();
        while(iterator.hasNext()){
            anchorPane.getChildren().remove(carMap.get(iterator.next()));
        }
        try{
            finalAnimation.stop();
        }catch(IllegalStateException e){
        }
        finalAnimation.getChildren().clear();
        finalAnimation = null;
        finalAnimation = new SequentialTransition();
        comboCars.setDisable(false);
        startButton.setDisable(false);
        skipButton.setDisable(true);
        carMap = new HashMap<>();
        for(int i=0;i<objectsInAnchor.size();i++){
            anchorPane.getChildren().remove(objectsInAnchor.get(i));
        }
        objectsInAnchor.clear();
        String value = comboCars.getValue();
        comboCars.setValue("");
        comboCars.setValue(value);
        finalMessage = "";
        carsInsideLabel.setText("Cars inside: 0");
    }

    @FXML
    public void showHelp(ActionEvent event){
        String title = "AYUDA";
        String message = "Aplicación hecha por: RockLee444";

        showFirstAlert(title,message, Alert.AlertType.INFORMATION);
    }

    private void showAlert(String title, String message, Alert.AlertType alertType){
        Alert alert = new Alert(alertType);
        alert.setHeaderText(null);
        alert.setTitle(title);

        TextArea area = new TextArea(message);
        area.setWrapText(true);
        area.setEditable(false);

        alert.getDialogPane().setContent(area);
        alert.getDialogPane().getContent().setStyle("-fx-background-color: transparent;");
        alert.setResizable(true);
        alert.initStyle(StageStyle.UTILITY);
        alert.show();
    }

    private  void showFirstAlert(String title, String message, Alert.AlertType alertType){
        Alert alert = new Alert(alertType);
        alert.setHeaderText(null);
        alert.setTitle(title);
        alert.setContentText(message);
        alert.show();
    }
}
