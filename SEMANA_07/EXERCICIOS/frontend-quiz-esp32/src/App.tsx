import * as React from "react";
import {
  ChakraProvider,
  Box,
  Text,
  VStack,
  Grid,
  theme,
  HStack,
  RadioGroup,
  Stack,
  Radio,
  Button,
} from "@chakra-ui/react";
import { ColorModeSwitcher } from "./ColorModeSwitcher";
import { useEffect, useState } from "react";
import { finishGame } from "./esp";
import { questions } from "./questions";

export const App = () => {
  const [playerId, setPlayerId] = useState(-1);
  const [points, setPoints] = useState(0);
  const [questionId, setQuestionId] = useState(0);
  const [selectedOptionId, setSelectedOptionId] = useState("0");

  const [currentQuestionObj, setCurrentQuestionObj] = useState({
    text: "This is a template question",
    options: [
      { name: "first option", points: 0 },
      { name: "second option", points: 0 },
      { name: "third option", points: 30 },
      { name: "fourth option", points: 0 },
    ],
  });

  const [finished, setFinished] = useState(false);

  useEffect(() => {
    if (finished) {
      console.log("Finished");
      console.log(points);
      finishGame(playerId, points, 0);
    }
  }, [finished]);

  useEffect(() => {
    setCurrentQuestionObj(questions[questionId]);
    setQuestionId(questionId + 1);
  }, []);

  const nextQuestionHandler = () => {
    const selectedOption = Number(selectedOptionId);
    const amount = questions[questionId - 1].options[selectedOption].points;
    setPoints(points + amount);
    if (questionId == questions.length) {
      setFinished(true);
    } else {
      setCurrentQuestionObj(questions[questionId]);
      setQuestionId(questionId + 1);
      setSelectedOptionId("0");
    }
  };

  return (
    <ChakraProvider theme={theme}>
      <Box textAlign="center" fontSize="xl">
        <Grid minH="100vh" p={3}>
          <ColorModeSwitcher justifySelf="flex-end" />
          {playerId == -1 ? (
            <>
              <VStack>
                <Text fontSize={"2xl"}>Selecione um dos players</Text>
                <Text fontSize={"md"}>Pressione o botão físico para resetar a partida!</Text>
                <HStack spacing={3}>
                  <Button
                    colorScheme="blue"
                    onClick={() => {
                      setPlayerId(0);
                    }}
                  >
                    Player Azul
                  </Button>
                  <Button
                    colorScheme="red"
                    onClick={() => {
                      setPlayerId(1);
                    }}
                  >
                    Player Vermelho
                  </Button>
                </HStack>
              </VStack>
            </>
          ) : (
            <VStack spacing={8}>
              {!finished ? (
                <>
                  <HStack>
                    <Text fontSize={"3xl"}>{questionId}.</Text>
                    <Text fontSize={"2xl"}>{currentQuestionObj.text}</Text>
                  </HStack>
                  <RadioGroup
                    defaultValue="0"
                    onChange={setSelectedOptionId}
                    value={selectedOptionId}
                  >
                    <Stack>
                      <Radio value="0">
                        a) {currentQuestionObj.options[0].name}
                      </Radio>
                      <Radio value="1">
                        b) {currentQuestionObj.options[1].name}
                      </Radio>
                      <Radio value="2">
                        c) {currentQuestionObj.options[2].name}
                      </Radio>
                      <Radio value="3">
                        d) {currentQuestionObj.options[3].name}
                      </Radio>
                    </Stack>
                  </RadioGroup>
                  <Button colorScheme="blue" onClick={nextQuestionHandler}>
                    Responder
                  </Button>
                </>
              ) : (
                <Text fontSize={"1xl"}>Player {playerId + 1} finalizado!</Text>
              )}
            </VStack>
          )}
        </Grid>
      </Box>
    </ChakraProvider>
  );
};
