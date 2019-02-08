open BsReactstrap;
open Utils;
open Emotion;
open ReactMotionBinding;

/*
 let trackList = [|
   "1",
   "2",
   "3",
   "4",
   "5",
   "6",
   "7",
   "8",
   "9",
   "10",
   "11",
   /*  "12",*/
 |];*/

let wrapper = [%css
  [
    display(`flex),
    height(`px(300)),
    position(`relative),
    width(`vw(90.)),
    border(`px(1), `solid, `hex("fff")),
    margin(`auto),
    overflow(`scroll),
    perspective(`px(1000)),
  ]
];

let trackStyle = [%css
  [
    /*    backgroundColor(`hex("fff")),*/
    color(`hex("fff")),
    width(`px(200)),
    height(`px(300)),
    position(`absolute),
    margin(`px(16)),
    flexShrink(0.),
    /*right(Calc.(`vw(-45.) - `px(1200))),*/
    transformStyle(`preserve3d),
    zIndex(2),
  ]
];

let trackStyleHidden = [%css
  [
    backgroundColor(`hex("123")),
    color(`hex("000")),
    width(`px(200)),
    height(`px(200)),
    position(`relative),
    margin(`px(16)),
    flexShrink(0.),
    right(`px(0)),
    /* visibility(`hidden),*/
  ]
];

type state = {
  focusedIdx: int,
  scrollLeft: float,
  initialScrollLeft: float,
  trackList: array(string),
};

type action =
  | UpdateFocusedId
  | HandleScroll
  | SetInitialScroll
  | UpdateTrackList(array(string));

let component = ReasonReact.reducerComponent("TrackhistoryListDisplay");

let getAimationStyleValue =
    (trackList, focusedIdx, initialScroll, currentScroll) => {
  let returnFunction = () =>
    trackList
    |> Array.mapi((idx, _value) =>
         if (idx === focusedIdx) {
           let offSetX = 0.1 -. (initialScroll -. currentScroll);

           let scale = 1.3;
           let rotate = 0.1;
           {
             x: spring(offSetX, gentle),
             scale: spring(scale, gentle),
             rotate: spring(rotate, gentle),
           };
         } else {
           /*`200. /. 2.` is the half width of the square*/
           let offSetX =
             float_of_int(idx - focusedIdx)
             *. (200. +. 32.)
             -. (initialScroll -. currentScroll);
           let scale = 1.1;
           let rotate = (-40.0);
           {
             x: spring(offSetX, gentle),
             scale: spring(scale, gentle),
             rotate: spring(rotate, gentle),
           };
         }
       );

  returnFunction;
};

/*  "transform": "traslate(" ++ xVal ++ "px, 0px) scale(" ++ scale ++ ")",*/

let trackAnimationStyle = (idx, focusedIdx, xVal, scale, rotate) =>
  if (idx === focusedIdx) {
    let newScale =
      scale === 1.0 ? string_of_float(1.1) : string_of_float(scale);
    let newXVal =
      scale === 1.0 ? string_of_float(1.1) : string_of_float(xVal);
    ReactDOMRe.Style.make(
      ~transform=
        "translate("
        ++ newXVal
        ++ "px, 0px) scale("
        ++ newScale
        ++ ") rotateY("
        ++ string_of_float(rotate)
        ++ "deg)",
      (),
    );
  } else {
    let newScale =
      scale === 1.0 ? string_of_float(1.1) : string_of_float(scale);
    ReactDOMRe.Style.make(
      ~transform=
        "translate("
        ++ string_of_float(xVal)
        ++ "px, 0px) scale("
        ++ newScale
        ++ ") rotateY("
        ++ string_of_float(-40.1)
        ++ "deg)",
      (),
    );
  };

let make = (~trackList, _children) => {
  ...component,
  initialState: () => {
    focusedIdx: 10,
    scrollLeft: 10000.,
    initialScrollLeft: 10000.,
    trackList,
  },
  reducer: (action, state) =>
    ReasonReact.(
      switch (action) {
      | UpdateFocusedId => Update({...state, focusedIdx: state.focusedIdx})
      | HandleScroll =>
        if (abs_float(state.initialScrollLeft -. 10000.) < 30.) {
          SideEffects((self => self.send(SetInitialScroll)));
        } else {
          let scrollLeft = getElementById("trackListWrapper") |> scrollLeft;
          let threshold = 200.;
          Js.log2("state scrolling", state.scrollLeft);
          Js.log2("current scrolling", scrollLeft);
          Js.log2("diff", state.scrollLeft -. scrollLeft);
          if (state.scrollLeft -. scrollLeft > threshold) {
            Update({...state, scrollLeft, focusedIdx: state.focusedIdx - 1});
          } else if (state.scrollLeft -. scrollLeft < (-1.0) *. threshold) {
            Update({...state, scrollLeft, focusedIdx: state.focusedIdx + 1});
          } else {
            NoUpdate;
          };
        }

      | SetInitialScroll =>
        let initialScrollLeft =
          getElementById("trackListWrapper") |> scrollLeft;
        Js.log2("current scrolling", scrollLeft);
        Update({...state, initialScrollLeft});
      | UpdateTrackList(trackList) =>
        Js.log("updatting...");
        Update({...state, trackList});
      }
    ),
  didMount: self => {
    scrollTo(getElementById("trackListWrapper"), 10000, 0);
    self.send(SetInitialScroll);
  },
  willUpdate: ({oldSelf, newSelf}) => {
    Js.log(trackList);
    if (Array.length(newSelf.state.trackList) !== Array.length(trackList)) {
      oldSelf.send(UpdateTrackList(trackList));
    };
  },
  render: self =>
    ReasonReact.(
      <div>
        <div>
          {
            Array.length(self.state.trackList) > 0 ?
              string(self.state.trackList[0]) : string("NONONONONO")
          }
        </div>
        <ReactMotionBinding
          className="motion"
          getAimationStyleValue={
            getAimationStyleValue(
              self.state.trackList,
              self.state.focusedIdx,
              self.state.initialScrollLeft,
              self.state.scrollLeft,
            )
          }
          defaultStyles={
            self.state.trackList
            |> Array.map(_trackId => {x: 0., scale: 1., rotate: 0.})
          }>
          {
            styleValue =>
              <div
                className=wrapper
                id="trackListWrapper"
                onClick={e => Js.log(e)}
                onScroll={_e => self.send(HandleScroll)}>
                /*                <div className={Cn.make([trackStyleHidden])}>
                                    {string("hidden")}
                                  </div>
                                  <div className={Cn.make([trackStyleHidden])}>
                                    {string("hidden")}
                                  </div>
                                  <div className={Cn.make([trackStyleHidden])}>
                                    {string("hidden")}
                                  </div>
                                  <div className={Cn.make([trackStyleHidden])}>
                                    {string("hidden")}
                                  </div>
                                  <div className={Cn.make([trackStyleHidden])}>
                                    {string("hidden")}
                                  </div>
                                  <div className={Cn.make([trackStyleHidden])}>
                                    {string("hidden")}
                                  </div>*/

                  {
                    self.state.trackList
                    |> Array.mapi((idx, trackId) =>
                         <div
                           key={string_of_int(idx)}
                           className={Cn.make([trackStyle])}
                           id={"number_" ++ string_of_int(idx)}
                           style={
                             trackAnimationStyle(
                               idx,
                               self.state.focusedIdx,
                               Array.length(styleValue) - 1 >= idx ?
                                 styleValue[idx].x : 1.1,
                               Array.length(styleValue) - 1 >= idx ?
                                 styleValue[idx].scale : 1.1,
                               Array.length(styleValue) - 1 >= idx ?
                                 styleValue[idx].rotate : 1.1,
                             )
                           }>
                           <GetTrackInfoQuery trackId>
                             ...{
                                  ({songName, artistName, albumImageUrl}) =>
                                    <SimpleTrack
                                      songName
                                      artistName
                                      albumImageUrl
                                      newestHistory={
                                        idx
                                        === Array.length(self.state.trackList)
                                        - 2 ?
                                          true : false
                                      }
                                      isCurrentTrack=false
                                      idx
                                    />
                                }
                           </GetTrackInfoQuery>
                         </div>
                       )
                    |> ReasonReact.array
                  }
                  {
                    /*styleValue*/
                    Array.mapi(
                      (i, style: ReactMotionBinding.style) =>
                        <div
                          key={string_of_int(i)}
                          className={Cn.make([trackStyle])}
                          id={"number_" ++ string_of_int(i)}
                          style={
                            trackAnimationStyle(
                              i,
                              self.state.focusedIdx,
                              style.x,
                              style.scale,
                              style.rotate,
                            )
                          }>
                          {string(string_of_int(i) ++ "-")}
                          {string(string_of_float(style.x))}
                        </div>,
                      styleValue,
                    )
                    |> ReasonReact.array
                  }
                  {
                    styleValue
                    |> Js.Array.mapi((style: ReactMotionBinding.style, i) =>
                         <div>
                           <div className={Cn.make([trackStyleHidden])}>
                             {string(string_of_int(i) ++ "-")}
                             {string(string_of_float(style.x))}
                           </div>
                         </div>
                       )
                    |> ReasonReact.array
                  }
                </div>
          }
        </ReactMotionBinding>
      </div>
    ),
};
